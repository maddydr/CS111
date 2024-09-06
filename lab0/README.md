# A Kernel Seedling
This project involves creating a kernel module that counts the number of running processes and makes the count available through the /proc/count file

## Building
To build the kernel module, navigate to the project directory and run:
```shell
make
```

## Running
To insert the kernel module into the kernel, use the following command
```shell
sudo insmood proc_count.ko
cat /proc/count
```
To check the number of current running processes, the cat command will read the /proc/count file
The output will be a single integer representing the number of running processes

## Cleaning Up
To remove the kernel module from the kernel, use
```shell
sudo rmmod proc_count
make clean
```

## Testing
```python
python -m unittest
```
Results after running this python command will display the results of the unit tests. Results were:
...
----------------------------------------------------------------------
Ran 3 tests in 2.847s

OK

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
Results: Linux 5.14.8-arch1-1 #1 SMP PREEMPT Sun, 26 Sep 2021 19:36:15 +0000