# Cache MOESIF Simulation
Simulates cache coherency and cache management in a modern multi-core system. Designed an LRU-based MOESIF cache coherency protocol for a four-core machine and reported various statistics including the

Number of cache hits
Number of cache misses
Number of writebacks
Number of broadcasts
Number of cache-to-cache transfers

The design has four cores, each with only one level of private cache. For simplicity, each cache has only four sets (one byte per set) and it is fully associative. It is writeback, write-allocate. Each set in the cache stores four values: A coherency state, an LRU state, a dirty bit, and a tag.

## Replacement Policy: 
(a) Find the first invalid line and install the line there. Update the LRU. 
(b) If no line is invalid, find the LRU state=0, and evict the line. Update the LRU (issue a writeback if needed).

## Running the Program
Compile with 
```shell
g++ *.cpp -o coherentsim
```

Run a sequence of reads and writes with the core ID and the tag/address in a text file:
```shell
./coherentsim <inputfile.txt>
```

A few example text files are given, with the expected output in the "expected_output" folder.

