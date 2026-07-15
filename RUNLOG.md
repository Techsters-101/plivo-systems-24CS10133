# Experiment Log

**Experiment 1**
* Profile: A.json
* Delay: 80ms
* Miss %: 2.20%
* Overhead: 1.22x
* Change: Initial implementation of reactive ARQ protocol and jitter buffer.
* Why: To establish a baseline and test if an 80ms window is wide enough for a full NACK request and retransmission round-trip. (Result: INVALID, window too tight).

**Experiment 2**
* Profile: A.json
* Delay: 120ms
* Miss %: 0.00%
* Overhead: 1.23x
* Change: Increased `delay_ms` to 120.
* Why: To give the network enough time to complete the NACK round-trip and recover dropped packets before the playout deadline. (Result: VALID).

**Experiment 3**
* Profile: A.json
* Delay: 100ms
* Miss %: 0.47%
* Overhead: 1.23x
* Change: Decreased `delay_ms` to 100.
* Why: To optimize our primary scoring metric (lowest playout delay) while maintaining a deadline miss rate safely below the 1.00% cap. (Result: VALID).

**Experiment 4**
* Profile: A.json
* Delay: 90ms
* Miss %: 1.20%
* Overhead: 1.22x
* Change: Decreased `delay_ms` to 90.
* Why: Pushing the limits of the network RTT to find the absolute lowest delay. (Result: INVALID, miss rate exceeded 1%). Locked in final score at 100ms.

**Experiment 5**
* Profile: B.json
* Delay: 300ms
* Miss %: 0.00%
* Overhead: 1.36x
* Change: Throttled receiver NACK spam from 5ms to 25ms. Increased delay to 300ms.
* Why: Profile B's heavy burst losses caused our aggressive 5ms NACKs to exceed the 2.0x bandwidth cap. Throttling fixed the bandwidth, and 300ms gave the network enough time to recover the massive packet drops. (Result: VALID).

**Experiment 6**
* Profile: B.json
* Delay: 200ms
* Miss %: 2.53%
* Overhead: 1.37x
* Change: Decreased `delay_ms` to 200.
* Why: Attempting to find the lowest possible delay for the burst-loss profile. (Result: INVALID, miss rate exceeded 1%).

**Experiment 7**
* Profile: B.json
* Delay: 250ms
* Miss %: 0.20%
* Overhead: 1.36x
* Change: Increased `delay_ms` to 250.
* Why: Bracketing the delay between 200ms and 300ms to lock in our absolute best score for Profile B. (Result: VALID). Locked in final stress-tested score at 250ms.