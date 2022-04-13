# Abstract

This is an implementation of Hash table. The table is used to study Hashing algorithms collisions and performance. 
In the later versions, AVX instructions are to be used for performance boost.

# Usage

The Hash Table itself does not provide any hashing functions. A few of them are implemented in Hashing.cpp. The Table expects a hashing function to be
```int64_t Hash (const void *key, int key_len)```

Init the table with ```Hash_t [name] = {};```
Then use a constructor to prepare it properly;

To be continued...
