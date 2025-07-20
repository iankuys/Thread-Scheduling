# Real-Time Thread Scheduler Simulator (C++ | Pthreads)

This project simulates a **real-time thread scheduling system** using native POSIX threads (`pthreads`) in C++. It implements three fundamental scheduling strategies:
- **FIFO (First-In-First-Out)**
- **EDF (Earliest Deadline First)**
- **RM (Rate Monotonic Scheduling)**

The goal is to emulate real-time operating system behavior, manually controlling when threads are allowed to run, preempt, or terminate — rather than relying on the OS scheduler.

---

## Features

- **Thread lifecycle control** with `pthread_cond_t` and `pthread_mutex_t`
- Simulation of three major real-time scheduling algorithms
- **Deadline and period enforcement** via timestamp comparison
- **Preemption handling** and safe task switching
- Logs and traces to validate scheduler behavior over 240 intervals
- Thread coordination via `resume`, `init`, and `preempt` conditions

## Scheduling Logic

### FIFO (First-In-First-Out)
- Picks the first thread from the ready queue.
- No preemption — the current thread runs until it completes its task.

### EDF (Earliest Deadline First)
- Always runs the thread with the **earliest deadline**.
- Supports **preemption** — if a new thread arrives with a closer deadline, the current task is interrupted.

### RM (Rate Monotonic)
- Priority is based on the **shortest period**.
- Like EDF but with static priority assignment.
- Requires manual implementation of preemptive logic (optional for extra credit).

---

## Build & Run

### Requirements
- Linux/macOS (POSIX-compatible)
- `g++` with `-pthread` and C++17 support

### Compile & Run
```bash
make
./main <0|1|2>
# 0 = FIFO, 1 = EDF, 2 = RM
