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
    2) For each word of the book, call TableFind 512 times
    3) Erase the whole book word by word

* Judging by the Callgrind output, the slowest function was TableFind itself, as it
does a lot of safety checks in runtime. So we have decided to optimize it first.

The first step was to reduce the number of conditional jumps as they are highly inefficient.
This was done by replacing conditional jumps with conditional moves.

This reduced the number of cycles TableFind is executed from 1.6 Bil to 800 Mil.

After this, we have found that the prologue of the TableFind function requires significant time. So the next step to optimize it was making this function inline.

Making it inline impacted slightly overall performance. Table of the ```main``` fucntion performance:

| Inline? | Instructions, Bil  | Exec. Time, s |
|:-------:|:------------------:|:-------------:|
|   NO    |         2.7        |     5.170     |
|   YES   |         3.0        |     5.070     |

* The next function to optimize was MurmurHash. We tried using djb2 hash function instead of Murmur. The amount of instructions executed was reduced. However, overall execution time suffered from this:

| Hash Function | Instructions, Mil | Exec. Time, s |
|:-------------:|:-----------------:|:-------------:|
|    Murmur     |        780        |     5.100     |
|    djb2       |        690        |     7.900     |

![mur](https://user-images.githubusercontent.com/52855633/165120395-f061d32a-b027-4bf7-abe2-f713c9570680.png)

![djb2](https://user-images.githubusercontent.com/52855633/165120491-d74a0df1-0341-4904-9add-f1df32d215c7.png)

Note: Although the number of cycles reduced when using djb2 hash, execution time increased.

We have tried to improve execution time by rewriting MurmurHash in Assembly language.
However, this did only reduce the performance of the program:

| Assembly | Exec. Time, s |
|:--------:|:-------------:|
|   NO     |     6.400     |
|   YES    |     6.700     |

Result: Hash function is working almost perfectly, no need to optimize it.

* The most heavy function is now List Find, as it uses strncmp too many times. Zero step is
to replace strcmp with memcmp, as we already have length of each string.

| Comparator | Exec. Time, s |
|:----------:|:-------------:|
|   strcmp   |     5.100     |
|   memcmp   |     4.500     |

Now we need to replace memcmp for short words with AVX instruction to compare multiple
bytes at once.

This is a success! Performance imroved almost 1.5 times:

| Intrinsics | Exec. Time, s |
|:----------:|:-------------:|
|    NO      |     4.500     |
|    YES     |     2.900     |


* Another attempt on changing the hash function: Use intrinsics CRC32 hash.

| Intrinsics hash | Exec. Time, s |
|:---------------:|:-------------:|
|      NO         |     2.900     |
|      YES        |     2.400     |

This was the last optimization so far. Let us sum up.

# Optimization summary

* Inlining a function gave us 2% performance boost.
* Changing the Hash function as well as implementing it in ASM decreases the computation speed.
* Replacing strncmp with memcmp is a 13% boost.
* Implementing parallel memcmp made it 2 times faster.
* Finaly, replacing Murmur Hash with parallel CRC32 made our program 18% faster.

![final](https://user-images.githubusercontent.com/52855633/165120552-75fb9277-daa6-44ff-9665-918b9877c7f8.png)

* Overall speedup: 2.2x times
* 2.2 / 113 lines of assembly and SIMD code * 1000 = 18.8
