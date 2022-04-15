# Abstract

This is an implementation of Hash table. The table is used to study Hashing algorithms collisions and performance. 
In the later versions, AVX instructions are to be used for performance boost.

# Usage

The Hash Table itself does not provide any hashing functions. A few of them are implemented in Hashing.cpp. The Table expects a hashing function to be
```int64_t Hash (const void *key, int key_len)```

Init the table with ```Hash_t [name] = {};```
Then use a constructor to prepare it properly;

# Optimization history
* When the first version of the program was finished, we used Callgrind to profile its
performance. The stress test was the following:

    1) Load the whole Silmarillion by J.R. Tolkien into the hash table
    2) For each word of the book, call TableFind 100 times
    3) Erase the whole book word by word

* Judging by the Callgrind output, the slowest function was TableFind itself, as it
does a lot of safety checks in runtime. So we have decided to optimize it first.

The first step was to reduce the number of conditional jumps as they are highly inefficient.
This was done by replacing conditional jumps with conditional moves.

