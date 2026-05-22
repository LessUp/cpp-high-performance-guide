# Concurrency and Multithreading

## ADDED Requirements

### Requirement: Queue Payload Lifetime Safety

THE Example_Module SHALL allow the lock-free queue interface to store payload types without requiring a default constructor.

#### Scenario: MPMC queue accepts non-default-constructible payloads

- **WHEN** a maintainer instantiates `MPMCQueue<T, N>` with a payload type that is move-constructible but has no default constructor
- **THEN** the queue builds successfully and preserves payload values through push/pop operations

#### Scenario: MPMC queue preserves move-only payloads

- **WHEN** a maintainer enqueues move-only payloads into `MPMCQueue<T, N>`
- **THEN** the queue transfers ownership correctly and pops each payload exactly once
