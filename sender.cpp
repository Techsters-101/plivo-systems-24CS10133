#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <array>
#include <stdint.h>
#include <cstring>
#include <iostream>

using namespace std;

void set_nonblock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int main() {
    int in_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in in_addr = {0};
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(47010);
    in_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(in_fd, (sockaddr *)&in_addr, sizeof(in_addr));
    set_nonblock(in_fd);

    int nack_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in nack_addr = {0};
    nack_addr.sin_family = AF_INET;
    nack_addr.sin_port = htons(47004);
    nack_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(nack_fd, (sockaddr *)&nack_addr, sizeof(nack_addr));
    set_nonblock(nack_fd);

    int out_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in relay = {0};
    relay.sin_family = AF_INET;
    relay.sin_port = htons(47001);
    relay.sin_addr.s_addr = inet_addr("127.0.0.1");

    map<uint32_t, array<uint8_t, 164>> history;
    uint8_t buf[2048];

    while (true) {
        // 1. Process incoming packets from Harness
        ssize_t n = recvfrom(in_fd, buf, sizeof(buf), 0, nullptr, nullptr);
        if (n == 164) {
            uint32_t seq;
            memcpy(&seq, buf, 4);
            seq = ntohl(seq);
            
            array<uint8_t, 164> pkt;
            memcpy(pkt.data(), buf, 164);
            history[seq] = pkt;
            
            sendto(out_fd, buf, n, 0, (sockaddr *)&relay, sizeof(relay));
        }

        // 2. Process incoming NACKs from Receiver
        n = recvfrom(nack_fd, buf, sizeof(buf), 0, nullptr, nullptr);
        if (n == 4) {
            uint32_t nack_seq;
            memcpy(&nack_seq, buf, 4);
            nack_seq = ntohl(nack_seq);
            
            if (history.count(nack_seq)) {
                sendto(out_fd, history[nack_seq].data(), 164, 0, (sockaddr *)&relay, sizeof(relay));
            }
        }
        usleep(100); 
    }
    return 0;
}