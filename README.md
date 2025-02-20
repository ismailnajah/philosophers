# Dining Philosophers Problem

This repository contains an implementation of the Dining Philosophers problem in C, using different synchronization mechanisms:

- `philo/` - Implements the solution using **threads** and **mutexes**.
- `philo_bonus/` - Implements the solution using **processes** and **semaphores**.

## Table of Contents

- [Description](#description)
- [Compiling and Running](#compiling-and-running)
- [Usage](#usage)
- [Implementation Details](#implementation-details)
- [Files Structure](#files-structure)
- [License](#license)

## Description

The Dining Philosophers problem is a classical synchronization problem that illustrates challenges in concurrent programming. The goal is to prevent deadlocks and starvation while allowing each philosopher to eat and think efficiently.

### Rules:

- Each philosopher must alternately **eat, sleep** and **think**.
- To eat, a philosopher needs to pick up **two forks** (one on the left, one on the right).
- Only one philosopher can hold a fork at a time.
- The simulation should prevent **deadlocks** and **starvation**.

## Compiling and Running

### Compiling

To compile the **thread-based** solution:

```bash
cd philo
make
```

To compile the **process-based** solution:

```bash
cd philo_bonus
make
```

### Running

After compilation, run the executable with the following parameters:

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

For example:

```bash
./philo 5 800 200 200
```

This starts a simulation with:

- 5 philosophers
- 800 ms before a philosopher dies without eating
- 200 ms to eat
- 200 ms to sleep

The optional last argument sets the number of times each philosopher must eat before stopping the simulation.

## Implementation Details

### `philo/` (Threads & Mutexes)

- Each philosopher is represented as a **thread**.
- **Mutexes** are used to synchronize access to forks (shared resources).
- A global mutex protects shared data like printing logs.

### `philo_bonus/` (Processes & Semaphores)

- Each philosopher is represented as a **separate process**.
- **Semaphores** are used for fork access control and synchronization.
- A global semaphore manages printing operations to prevent race conditions.

## Files Structure

```
📂 philo/
 ├── lock.c
 ├── main.c
 ├── Makefile
 ├── monitor.c
 ├── parse.c
 ├── philo_actions.c
 ├── philo.c
 ├── philo.h
 ├── simulation.c
 ├── simulation_utils.c
 ├── time.c
 ├── utils.c

📂 philo_bonus/
 ├── lock_bonus.c
 ├── main_bonus.c
 ├── Makefile
 ├── monitors_bonus.c
 ├── parse_bonus.c
 ├── philo_actions_bonus.c
 ├── philo_bonus.c
 ├── philo_bonus.h
 ├── simulation_bonus.c
 ├── simulation_utils_bonus.c
 ├── time_bonus.c
 ├── utils_bonus.c
```

## License

This project is open-source and distributed under the MIT License.
