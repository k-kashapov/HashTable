#include <stdint.h>
#include <x86intrin.h>

int64_t SumHash      (const void *data_ptr, int len);
int64_t FirstSymHash (const void *data_ptr, int len);
int64_t StrLenHash   (const void *data_ptr, int len);
int64_t RoLHash      (const void *data_ptr, int len);
int64_t RoRHash      (const void *data_ptr, int len);
int64_t MurmurHash2A (const void *data_ptr, int len);
int64_t djb2Hash     (const void *data_ptr, int len);
int64_t MurmurHash2  (const void *data_ptr, int len);
int64_t CRC32        (const void *data_ptr, int len);
