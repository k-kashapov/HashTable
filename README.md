# Abstract

This is an implementation of Hash table. The table is used to study Hashing algorithms collisions and performance.
AVX instructions are used for performance boost.

# Usage

The Hash Table itself does not provide any hashing functions. A few of them are implemented in Hashing.cpp. The Table expects a hashing function to be
```int64_t Hash (const void *key, int key_len)```

Init the table with ```Hash_t table = {};```
Then use a constructor to prepare it properly.

# Uniformity test

The first part of the task was to check the uniformity of several hash functions:

1) Hash = String length
2) Hash = First symbol of the string
3) Sum of symbols' ASCII codes
4) ROL hash: ```Hash[i + 1] = (Hash[i] ROL 1) ^ String[i + 1]```
5) [MurmurHash2A](https://ru.wikipedia.org/wiki/MurmurHash2)
6) [CRC32 Hash](https://wiki.osdev.org/CRC32)

To test the function, we have hashed the entire [Silmarillion by J.R. Tolkien](https://en.wikipedia.org/wiki/The_Silmarillion) word by word.
Diagrams of Hash collisions were plotted. Amount of collisions of value ```H``` = length of the List, associated with hash value ```H```.
Chi-squared (more on that later) test was performed to estimate the uniformity quantitively.

# Experimental results

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

<img src="https://user-images.githubusercontent.com/52855633/168494851-3ce246a2-e7b0-400a-a2eb-0b269664681b.png" width = 70%>

CRC32 and MurmurHash are the most uniform hashes with values 1.06 and 1.07 respectively.

That concludes our research.

# Optimization history
* We want our HashTable to be used to search elements by key in long texts (more than 10000 words). As a result, the stress test was the following:

1) Load the whole Silmarillion by J.R. Tolkien into the hash table
2) For each word of the book, call ```TableFind``` 512 times
3) Erase the whole book from Table word by word

Peformance test were conducted using the [```perf```](https://perf.wiki.kernel.org/index.php/Tutorial) tool and Linux's ```time```. The number of cycles a function is executed and overall execution time are optimized. Optimization flag: ```-O2```.

In every measurement:
* Period     - approximate number of total CPU cycles a function has been executed. Output of ```perf```.
* Exec. Time - total time required to pass the stress test. Output of ```time```.

## TableFind optimization
* Judging by the ```perf``` output, the slowest function was TableFind itself, as it
does a lot of safety checks in runtime. So we have decided to optimize it first.

We have found that the prologue of the TableFind function requires significant time. So the first step is to make this function inline.

<img src="https://user-images.githubusercontent.com/52855633/168487189-b3dc61a0-fa07-42e2-98f0-48835ada5e79.png" width = 70%>

Inlining the function gave slight performance boost, but removed the function from the top of callgrind output. Table of the ```main``` function performance:

| Inline? |   Period, Bil. cycles   | Exec. Time, s |
|:-------:|:-----------------------:|:-------------:|
|   NO    |          5.6            |  3.31 ± 0.05  |
|   YES   |          4.4            |  3.03 ± 0.05  |

After inlining:

<img src="https://user-images.githubusercontent.com/52855633/168499995-9b32e464-4e26-44b2-ac6e-c92efe973666.png" width = 70%>

```CPU cycles for TableFind = Δ(cycles for StressTest)```, because they now form a single function.

## StressTest optimization
* The heaviest function is the StressTest, as it contains TableFind. Using ```perf```, we find out the most expensive part of this function is GetElemByHash, because it is called whenever a key is processed.

<img src="https://user-images.githubusercontent.com/52855633/168504447-34be53c4-1b42-4c2c-9895-8cf17167e541.png" width = 70%>

Our solution is to replace it with macro. 

```
void *GetElemByHash (Hash_t *target_table, int64_t hash)
{
    int64_t capacity_mask = target_table->capacity - 1;

    void *target_elem = target_table->Data[hash & capacity_mask];

    return target_elem;
}
```

Was replaced with

```
#define GET_ELEM_BY_HASH(tbl_, hash_) (tbl_->Data[hash_ & (tbl_->capacity - 1)])
```

However, it gives very little performance boost.

<img src="https://user-images.githubusercontent.com/52855633/168504481-045199f4-6410-40fc-9801-b74d95e43656.png" width = 70%>

|      Macro      |   Period, Bil. cycles   | Exec. Time, s |
|:---------------:|:-----------------------:|:-------------:|
|      NO         |           4.4           |  2.90 ± 0.08  |
|      YES        |           4.8           |  2.87 ± 0.05  |

Note: although the total amount of cycles is increased, execution time benefitted from the optimiztion.

At the current step we cannot find any improvements that could be done to speed up the TableFind function.
We will now try to optimize the second most heavy function: Hash.

## Hash optimization
* The next function to optimize was Hash function.

We have tried to improve execution time by rewriting [MurmurHash in Assembly language](https://github.com/k-kashapov/HashTable/blob/master/src/MurmurHash.s).
However, this did only reduce the performance of the program:

<img src="https://user-images.githubusercontent.com/52855633/168504759-e5ee229c-9ddc-4157-a432-f8711c04ee07.png" width = 70%>

| Assembly |    Period, Mil. cycles    | Exec. Time, s |
|:--------:|:-------------------------:|:-------------:|
|   NO     |           4.8             |   2.87 ± 0.08 |
|   YES    |           6.2             |   3.45 ± 0.01 |

* Another attempt on changing the hash function: Use [intrinsics CRC32 hash](https://github.com/k-kashapov/HashTable/blob/90f8478caadbb8fe4d78167d040d2f1136544fd5/src/Hashing.cpp#L190).

<img src="https://user-images.githubusercontent.com/52855633/168505485-8bf9fc1f-4795-4526-a2d9-d3b466c7e8bb.png" width=70%>

| Intrinsics hash |    Period, Mil. cycles    | Exec. Time, s |
|:---------------:|:-------------------------:|:-------------:|
|      NO         |            2.4            |   2.87 ± 0.08 |
|      YES        |            1.2            |   2.72 ± 0.01 |

Intrinsics Hash gave 6% performance boost.

Such small gain can be explained by the fact that hash function is not the bottleneck.

## StrCmp optimizations

From the following screenshot we can see, that the most time-consuming part of the ListFind function is the ```strcmp```.

<img src="https://user-images.githubusercontent.com/52855633/168487770-ea7c3308-1380-4478-84f9-cacdf0e5dbe4.png" width = 70%>

Zero step is to replace strcmp with memcmp, as we already have length of each string.

| Comparator | Exec. Time, s |
|:----------:|:-------------:|
|   strcmp   |  2.72 ± 0.01  |
|   memcmp   |  2.60 ± 0.02  |

Now we need to replace memcmp for short words with AVX instruction to compare multiple
bytes at once.

The results are surprisingly good! Performance has been improved by almost 30%:

<img src = "https://user-images.githubusercontent.com/52855633/168509254-585cb4d8-3d26-4c57-bf2d-6967482d491d.png" width = 70%>

| Intrinsics memcmp |    Period, Mil. cycles    | Exec. Time, s |
|:-----------------:|:-------------------------:|:-------------:|
|        NO         |           2.1             |  2.60 ± 0.02  |
|        YES        |           2.9             |  2.12 ± 0.02  |

Note: once again, CPU cycles do not corellate with overall performance trend.

# Inline ASM optimization

As a part of our course we were recommended to use inline ```__asm``` feature. This does not improve the execution speed. Education purposes only.

```
if (target_list->size > 1)
{
    long res_elem = ListFind (target_list, type_t { 0, key, key_len });
            
    if (res_elem) found = GET_LIST_DATA (target_list, res_elem);
}
```

Has been replaced with

```
__asm__ ("cmp $1, %0\n"
         "jle EmptyList\n"
         :: "r" (target_list->size));

            long res_elem = ListFind (target_list, type_t { 0, key, key_len });
            if (res_elem) found = GET_LIST_DATA (target_list, res_elem);
            
__asm__ ("EmptyList:\n" ::);
```

This was the last optimization so far. Let us sum up.

# Optimization summary

* ```perf``` killed my processor several times. It was very scary...
* Inlining a function gave us a barely noticable 2% performance boost.
* Changing the Hash function as well as implementing it in ASM decreases the computation speed.
* Replacing Murmur Hash with parallel CRC32 made our program 6% faster.
* Finaly, implementing parallel memcmp made it almost 1.3 times faster.

* Overall speedup: 1.56x times
    
## Optimization coefficient
As a part of our course we were recommended to count the following value:
* 1.56 times performance boost / 113 lines of assembly and SIMD code * 1000 = 13.8

# Acknowledgements

Big thanks to [Futherus](https://github.com/futherus), [Denchik](https://github.com/d3phys) and [Vasilmao](https://github.com/vasilmao) for reviewing my README file.
I would also like to express my gratitude to the entire development team of ```perf```.
