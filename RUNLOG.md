# Experiment Log

**Experiment 1**
* Profile: A.json
* Delay: 80ms
* Miss %: 2.20%
* Overhead: 1.22x
* Change: Initial implementation of our reactive ARQ protocol and jitter buffer.
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