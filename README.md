# eBPF Atomic Operations

Atomic operations refers to atomic CPU instructions. 

A normal `i += 1` operation will at some level break down into:

- Read i into some CPU register
- Increment the CPU register with 1
- Write the register value back to i

Since this happens in multiple steps, even such a simple operation is subject to a race condition.

There is a class of CPU instructions that can perform specific tasks in a single CPU instruction which is serialized at the hardware level. These are also available in BPF. 
When compiling with Clang/LLVM these special instructions can be accessed via a list of special builtin functions:

- `__sync_fetch_and_add(*a, b)` - Read value at `a`, add `b` and write it back, return the new value
- `__sync_fetch_and_sub(*a, b)` - Read value at `a`, subtract a number and write it back, return the new value
- `__sync_fetch_and_or(*a, b)` - Read value at `a`, binary OR a number and write it back, return the new value
- `__sync_fetch_and_xor(*a, b)` - Read value at `a`, binary XOR a number and write it back, return the new value
- `__sync_val_compare_and_swap(*a, b, c)` - Read value at `a`, check if it is equal to `b`, if true write `c` to a and return the original value of `a`. On fail leave `a` be and return `c`.
- `__sync_lock_test_and_set(*a, b)` - Read value at a, write b to a, return original value of a

If you want to perform one of the above sequences on a variable you can do so with the atomic builtin functions.

In the `atomic.c` you will find a dummy example how to use them.

**NOTE:** Atomic instructions work on variable of 1, 2, 4, or 8 bytes. Any variables larger than that such as multiple struct fields require multiple atomic instructions or other synchronization mechanisms.
