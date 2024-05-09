## Optimization Journey

This code has undergone several optimizations to improve its performance and efficiency. Here's a brief overview of the changes made:

### **Version 1: Unoptimized**

The initial version of the code was written with simplicity and readability in mind. It used a linear search to find the city in the results vector, which had a time complexity of O(n). This version was slow and inefficient.

### Version 2: Hash-based indexing


In the second version, a hash table was introduced to quickly find the index of a city in the results vector. This optimization reduced the time complexity of the city lookup to O(1) on average, using a simple hash function and linear probing to resolve collisions.


### Version 3: Manual memory management and custom parsing


In the third version, manual memory management was introduced to reduce memory allocation and deallocation overhead. A custom parseDouble function was also implemented to parse the measurement values more efficiently, using a simple parsing algorithm to extract the numeric value.


### Version 4: Multi-threading and chunk-based processing

In the final version, multi-threading was introduced to process the file in parallel. The file was divided into chunks, and each chunk was processed by a separate thread. This optimization significantly improved performance on multi-core systems, using a thread pool to manage the concurrent execution of tasks.

#### _Additional Optimizations_

- Memory-mapped file I/O was used to reduce file I/O overhead.
- memchr was used instead of string::find to find newline characters in the file.
- memcpy was used instead of string::assign to copy city names into the info struct.
- unordered_map was used instead of vector to store the city groups, providing faster lookup and insertion times.
- mutex was used to protect the Result struct from concurrent access by multiple threads.
- constexpr was used to evaluate constants at compile-time, reducing runtime overhead.
### Conclusion

Through a series of optimizations, the code has evolved from a simple but slow implementation to a highly efficient and optimized version. These optimizations have significantly improved the performance of the code, making it suitable for large datasets.
