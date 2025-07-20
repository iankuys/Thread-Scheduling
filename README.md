# C++ Thread Scheduling Simulator

This project implements a **custom thread scheduling system** in C++, simulating how threads are created, coordinated, and executed under controlled scheduling policies. Instead of relying purely on native OS scheduling, this project demonstrates **manual scheduling logic**, making it ideal for educational or systems-level use.

---

## Project Highlights

- Custom thread lifecycle management (`READY`, `RUNNING`, `WAITING`, etc.)
- Simulated scheduling logic (e.g., round-robin, FIFO, or custom policy)
- Shared data and synchronization between threads
- Modular architecture for future extension (thread pool, priorities, blocking queues)

---

## Components

- `main.cpp`: Program entry point and scheduler setup
- `p3_threads.cpp/h`: Core thread control logic — creation, state transitions, scheduler hooks
- `types_p3.cpp/h`: Custom types for thread states, IDs, control blocks
- `utils.cpp/h`: Timing utilities, debugging output, and helpers
- `Makefile`: Build configuration

---

## 🔧 Build & Run

```bash
make
./main
