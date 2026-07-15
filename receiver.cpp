#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <array>
#include <stdint.h>
#include <cstring>
#include <chrono>

using namespace std;

void set_nonblock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int main() {
    int in_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in in_addr = {0};
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(47002);
    in_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(in_fd, (sockaddr *)&in_addr, sizeof(in_addr));
    set_nonblock(in_fd);

    int player_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in player = {0};
    player.sin_family = AF_INET;
    player.sin_port = htons(47020);
    player.sin_addr.s_addr = inet_addr("127.0.0.1");

    int nack_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in nack_relay = {0};
    nack_relay.sin_family = AF_INET;
    nack_relay.sin_port = htons(47003);
    nack_relay.sin_addr.s_addr = inet_addr("127.0.0.1");

    map<uint32_t, array<uint8_t, 164>> buffer;
    uint32_t expected_seq = 0;
    uint32_t max_seen_seq = 0;
    auto last_nack_time = chrono::steady_clock::now();

    uint8_t buf[2048];

    while (true) {
        // 1. Receive data from network
        ssize_t n = recvfrom(in_fd, buf, sizeof(buf), 0, nullptr, nullptr);
        if (n == 164) {
            uint32_t seq;
            memcpy(&seq, buf, 4);
            seq = ntohl(seq);
            
            array<uint8_t, 164> pkt;
            memcpy(pkt.data(), buf, 164);
            buffer[seq] = pkt;

            if (seq > max_seen_seq) {
                max_seen_seq = seq;
            }
        }

        // 2. Push contiguous packets to the player
        while (buffer.count(expected_seq)) {
            sendto(player_fd, buffer[expected_seq].data(), 164, 0, (sockaddr *)&player, sizeof(player));
            buffer.erase(expected_seq);
            expected_seq++;
        }

        // 3. Spam NACKs for any gaps every 5ms
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(now - last_nack_time).count() > 5) {
            for (uint32_t s = expected_seq; s < max_seen_seq; s++) {
                if (!buffer.count(s)) {
                    uint32_t net_s = htonl(s); 
                    sendto(nack_fd, &net_s, 4, 0, (sockaddr *)&nack_relay, sizeof(nack_relay));
                }
            }
            last_nack_time = now;
        }
        
        usleep(100); 
    }
    return 0;
}