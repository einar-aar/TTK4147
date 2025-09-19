### Task A

- **What happens when allocating the 4GB matrix?**
  - The allocations may appear to succeed without immediately consuming 4GB of physical RAM. On Linux, overcommit and lazy allocation let `malloc` return virtual memory without backing it by physical pages until the memory is touched. You may not see a big increase in "Memory" until the program actually writes to the pages; failures or heavy swapping can happen once memory is used.

- **Difference between Memory and Swap**
  - **Memory**: physical RAM usage.
  - **Swap**: disk/flash-backed virtual memory used when RAM is insufficient; much slower than RAM.

- **Effect of increasing `x_dim` from 100 to 1000 or 10000**
  - Total bytes requested stay the same, but the shape changes. Larger `x_dim` means fewer rows and larger contiguous row allocations. Behavior can differ due to allocator/OS paging and fragmentation. Generally, with very large rows allocation/commit patterns and page faults may become more noticeable.

- **Why does initializing with `memset` change the behavior?**
  - `memset` touches every byte, forcing the OS to commit pages. This materializes the memory and increases actual RAM/Swap usage immediately, potentially causing failures or heavy swapping, unlike the uninitialized case where pages may remain uncommitted.

- **Does `malloc()` always allocate memory right away?**
  - No. On many systems it reserves virtual address space and defers physical page commitment until first access.


### Task B

- **What happens when inserting more than capacity without growth?**
  - It writes past the allocated buffer, causing undefined behavior. 

- **What happens with `-g -fsanitize=address` enabled?**
  - AddressSanitizer detects the out of bounds write and aborts with a detailed report pinpointing the invalid access.


### Task C

- **What does `array_reserve` do and how does growth work now?**
  - `array_reserve` allocates a larger buffer, copies existing elements contiguously starting at index 0, frees the old buffer, and updates `front`, `back`, and `capacity` accordingly. `array_insertBack` checks capacity and grows (here using a 2× policy) when needed, preserving amortized O(1) appends.


### Task D

- **2× growth (capacities: 2 → 4 → 8 → 16 → …)**
  - Each growth allocates a new block twice as large and copies data. Peak memory during relocation can transiently be old + new blocks if the old block isnt immediately reusable. These sizes often match allocator size classes, which can reduce fragmentation and improve reuse.

- **1.5× growth (capacities: 2 → 3 → 4 → 6 → 9 → 13 → 19 → …)**
  - More frequent relocations with smaller temporary spikes. Sizes are more varied, so reuse depends more on allocator behavior and may fragment more.

- **When can freed memory be reused?**
  - If the allocator can expand in place (adjacent space available), `realloc` may avoid moving. Freed blocks are quickly reusable when their sizes match future requests or size classes. Growth factors aligned with allocator classes (like powers of two) tend to improve reuse.

- **Trade-offs**
  - 2×: fewer copies (good amortized performance), higher peak memory during growth.
  - 1.5×: more copies (higher constant factor), lower per-step peak, potentially more fragmentation and less alignment with size classes.


