a)
Regarding using the dataset oneThousand.txt, it seems there is little difference between using 1 thread and 2, though performance worsens as more threads are used. When a very large number of threads are used, it seems doubling the number of threads doubles the execution time.

When using dataset oneMillion.txt, execution time seems to lower by a significant degree when using 2 or 3 threads, rather than 1. The improvement lessens when using 4 threads, and gets worse with 5 or more.
When using a very large number of threads, doubling the number of threads again roughly doubles the execution time.

b)
I would say the observed behavior is reasonable, 