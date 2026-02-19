a)
Regarding using the dataset oneThousand.txt, it seems there is little difference between using 1 thread and 2, though performance worsens as more threads are used. When a very large number of threads are used, it seems doubling the number of threads doubles the execution time.

When using dataset oneMillion.txt, execution time seems to lower by a significant degree when using 2 or 3 threads, rather than 1. The improvement lessens when using 4 threads, and gets worse with 5 or more.
When using a very large number of threads, doubling the number of threads again roughly doubles the execution time.

b)
I would say the observed behavior is reasonable. The purpose of using multiple threads is to be able to split one very large task into smaller tasks that can be completed concurrently.
It is reasonable to see no meaningful performance gain with the oneThousand.txt data, because the task is not very large, and overhead for establishing threads is more expensive than the task itself.
It is reasonable to see meaningful performance gains when using a small but multiple amount of threads with the oneMillion.txt data, because the task takes long enough to outweigh the overhead for establishing these threads.
It is also reasonable to see performance worsen when there are too many threads created, especially when it is a large number that is over the number of actual threads the CPU may have.

c)
It seems the Critical Section should house logic that deals with shared state across threads. Threads should only access shared state one at a time. If there is no lock, there would be a race condition, where two threads may read the same memory, but may overwrite each other. The Critical Section therefore should lock the shared where it is read and updated. This would mean reading totalSum and adding to it.

EXTRA)
I think that updating the shared totalSum element by element would very significantly increase execution time, because there would be very frequent locking and unlocking, which increases the amount of overhead experienced. For instance if two threads were used with the oneThousand.txt data, with the current approach unlocking and locking only occurs twice. With the alternative approach, unlocking and locking occurs one thousand times. Just for simplification, if this unlocking and locking took one second to do, then the first approach would only waste two seconds, whereas the second approach would waste 16 minutes.