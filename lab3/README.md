# Hash Hash Hash
This project involves making a hash table implementation thread-safe by adding mutex locks to ensure concurrent access does not result in data races or missing entries. The task involves modifying two versions of the hash table, focusing first on correctness with a single mutex, and then on both correctness and performance using multiple mutexes, followed by performance comparisons against a base implementation.

## Building
Run the following command in the directory with all the files. 
```shell
make
```

## Running
Run the executable with this command and optional arguments where -t changes the number of threads to use (default 4), and -s changes the number of hash table entries to add per thread (default
25,000). Example:
```shell
./hash-table-tester -t 4 -s 50000
Generation: 69,466 usec
Hash table base: 414,820 usec
  - 0 missing
Hash table v1: 380,722 usec
  - 0 missing
Hash table v2: 353,171 usec
  - 0 missing

```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex to the add_entry function. This was because the race condition existed in this section when 2 threads try to add to the hash table. This implementation solely focused on correctness, thus by locking the entire critical section, the threads are guaranteed to be safe with just this one mutex. 

### Performance
```shell
TODO how to run and results
```
Version 1 is a little slower than the base version. This makes sense because only one thread can run this critial section at a time. I locked the entire add_entry section, thus only one thread can execute and hold the mutex at a time, therefore making sense as to why it would be slower than the base. The overhead with the mutex added ultimately causes additional delays and waiting time, therefore slowing down performance. Parallelism is ultimately negated by the fact that they frequently block each other. This overhead becomes more noticeable as the number of threads increases since the threads spend more time waiting than doing actual work.

## Second Implementation
In the `hash_table_v2_add_entry` function, I implemented a per-bucket mutex for a more fine grain approach to locking. In this approach, each bucket in the hash table has its own mutex. This will allow different threads to access different buckets without all waiting, thus, significantly improving performance. We kept the same logic from V1 locking at the beginning before any operations on a specific bucket. Once the operation on a bucket is complete, it unlocks and allows other threads to access that bucket.

### Performance
Here is an example output of running:
```shell
./hash-table-tester -t 4 -s 50000
Generation: 69,466 usec
Hash table base: 414,820 usec
  - 0 missing
Hash table v1: 380,722 usec
  - 0 missing
Hash table v2: 353,171 usec
  - 0 missing
```
Based on these results, V2 is about 14.86% faster than the base, and about 7.25% faster than V1 using 4 cores and 50000 entries. This makes sense because in V2, our locking strategy improves parallelism leading to better performance. This hence meets the t-1 speedup requirement. 

TODO more results, speedup measurement, and analysis on v2

## Cleaning up
Run the following command to clean up the files in your directory
```shell
make clean
```