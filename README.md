# Abstract

This is an implementation of Hash table. The table is used to study Hashing algorithms collisions and performance.
AVX instructions are used for performance boost.

# Usage

The Hash Table itself does not provide any hashing functions. A few of them are implemented in Hashing.cpp. The Table expects a hashing function to be
```int64_t Hash (const void *key, int key_len)```

Init the table with ```Hash_t [name] = {};```
Then use a constructor to prepare it properly;

# Uniformity test

The first part of the task was to check the uniformity of several hash functions:

1) Hash = String length
2) Hash = First symbol of the string
3) Sum of symbols' ASCII codes
4) ROL hash: ```Hash[i + 1] = (Hash[i] ROL 1) ^ String[i + 1]```
5) MurmurHash2A
6) CRC32 Hash

To test the function, we have hashed the entire Silmarillion by J.R. Tolkien word by word.
Diagrams of Hash collisions were plotted. Amount of collisions of value H = length of the List, associated with hash value H.
Chi-squared test was performed to estimate the uniformity quantitively.

# Experimental results:

## 1) String length hash

Max collisions = 1594 at length = 6

<img src="https://user-images.githubusercontent.com/52855633/168428653-edf13266-c173-4817-b3c4-51cbc8c0ad8b.png" width = 70%>

## 2) Fist symbol hash

Max collisions = 717 at letter S

<img src="https://user-images.githubusercontent.com/52855633/168428691-4dfb5237-1c16-4f0a-a7f3-568745ffaec9.png" width = 70%>

## 3) Sum of symbols hash

Max value = 50

<img src="https://user-images.githubusercontent.com/52855633/168428698-0b9caf09-bdf4-4bd7-9eb6-6306a990df67.png" width = 70%>

## 4) ROL hash

Max value = 21

<img src="https://user-images.githubusercontent.com/52855633/168428705-7598c8d8-1a39-40ac-8a1b-06b09f61fcd9.png" width = 70%>

## 5) Murmur hash

Max value = 13

<img src="https://user-images.githubusercontent.com/52855633/168428714-481c3379-425d-4185-94cd-4df9ac1c46de.png" width = 70%>

## 6) CRC32 hash

Max value = 13

<img src="https://user-images.githubusercontent.com/52855633/168494613-e9568ccd-9ea2-406d-a543-297b87ba8935.png" width = 70%>

## Chi-squared test

[Chi-squared](https://en.wikipedia.org/wiki/Chi-squared_test) tests allows us to get the characteristic value of uniformity.
Values between 0.95 and 1.05 indicate highly uniform distribution.

The formula used:

<img src="https://user-images.githubusercontent.com/52855633/168495477-6f6aeb29-6d93-47bc-bfc5-0d958513ef8c.png" width = 35%>

Where 
* ```m``` - number of hash values
* ```n``` - number of occupated hash values
* ```b[j]``` - amount of collisions of value j

### Results

<img src="https://user-images.githubusercontent.com/52855633/168494851-3ce246a2-e7b0-400a-a2eb-0b269664681b.png" width = 50%>

CRC32 and MurmurHash are the most uniform hashes with values 1.06 and 1.07 respectively.

That concludes our research.

# Optimization history
* We want our HashTable to be used to search elements by key in long texts (more than 10000 words). As a result, the stress test was the following:

1) Load the whole Silmarillion by J.R. Tolkien into the hash table
2) For each word of the book, call ```TableFind``` 512 times
3) Erase the whole book from Table word by word

Peformance test were conducted using the ```perf``` tool and Linux's ```time```. The number of cycles a function is executed and overall execution time are optimized.

## TableFind optimization
* Judging by the ```perf``` output, the slowest function was TableFind itself, as it
does a lot of safety checks in runtime. So we have decided to optimize it first.

We have found that the prologue of the TableFind function requires significant time. So the first step is to make this function inline.

<img src="https://user-images.githubusercontent.com/52855633/168487189-b3dc61a0-fa07-42e2-98f0-48835ada5e79.png" width = 50%>

Inlining the function gave almost no performance boost, but removed the function from the top of callgrind output. Table of the ```main``` function performance:

| Inline? |   Period, Bil. cycles   | Exec. Time, s |
|:-------:|:-----------------------:|:-------------:|
|   NO    |          5.6            |  5.17 ± 0.2   |
|   YES   |          4.4            |  5.07 ± 0.2   |

After inlining:

<img src="https://user-images.githubusercontent.com/52855633/168499750-20edc128-2813-4574-81bf-1b9cd816efd0.png" width = 50%>

```CPU cycles for TableFind = Δ(cycles for StressTest)```, because they now form a single function.

## Hash optimization
* The next function to optimize was Hash function.

We have tried to improve execution time by rewriting MurmurHash in Assembly language.
However, this did only reduce the performance of the program:

| Assembly |    Period, Mil. cycles    | Exec. Time, s |
|:--------:|:-------------------------:|:-------------:|
|   NO     |                           |   6.40 ± 0.2  |
|   YES    |                           |   6.70 ± 0.2  |

## StrCmp optimizations
* At the time, we tried to optimize the second most heavy function: List Find. It is slow as it uses strncmp too many times.

From the following screenshot we can see, that the most time-consuming part of the ListFind function is the ```strcmp```.

<img src="https://user-images.githubusercontent.com/52855633/168487770-ea7c3308-1380-4478-84f9-cacdf0e5dbe4.png" width = 70%>

Zero step is to replace strcmp with memcmp, as we already have length of each string.

| Comparator | Exec. Time, s |
|:----------:|:-------------:|
|   strcmp   |  5.10 ± 0.2   |
|   memcmp   |  4.50 ± 0.2   |

Now we need to replace memcmp for short words with AVX instruction to compare multiple
bytes at once.

This is a success! Performance imroved almost 1.5 times:

| Intrinsics | Exec. Time, s |
|:----------:|:-------------:|
|    NO      |  4.50 ± 0.2   |
|    YES     |  2.90 ± 0.2   |
    
## One mode Hash optimization attempt
* Another attempt on changing the hash function: Use intrinsics CRC32 hash.

| Intrinsics hash | Exec. Time, s |
|:---------------:|:-------------:|
|      NO         |  2.90 ± 0.2   |
|      YES        |  2.40 ± 0.2   |

Intrinsics Hash gave 13% performance boost.

* The next function to optimize is GetElemByHash.
We have tried to optimize it, by replacing it with macro. However, it gave no performance boost.

![get_elem](https://user-images.githubusercontent.com/52855633/168489148-dd0af343-7394-445e-b4c2-fbdda554e4a5.png)

|      Macro      | Exec. Time, s |
|:---------------:|:-------------:|
|      NO         |  2.40 ± 0.2   |
|      YES        |  2.40 ± 0.2   |

This was the last optimization so far. Let us sum up.

# Optimization summary

* ```perf``` killed my processor once. It was very scary...
* Inlining a function gave us 2% performance boost.
* Changing the Hash function as well as implementing it in ASM decreases the computation speed.
* Replacing strncmp with memcmp is a 13% boost.
* Implementing parallel memcmp made it 2 times faster.
* Finaly, replacing Murmur Hash with parallel CRC32 made our program 18% faster.

<img src="https://user-images.githubusercontent.com/52855633/168489325-3f117c8a-9e16-4a58-bdba-bb75767f4d49.png" width = 70%>

* Overall speedup: 2.2x times
    
## Optimization coefficient
As a part of our course we were recommended to count the following value:
* 2.2 times performance boost / 113 lines of assembly and SIMD code * 1000 = 18.8
