1)

The number of CPUs in my system is 16
Therefore sched is run with 16, ./sched 16
It seems several of the threads oscillate between very low latency and very high latency, with a few of the threads stay more consistently at a higher latency, which maxes out the latency bar. Looking at Task Manager, the CPU is mostly fully utilized.

2)