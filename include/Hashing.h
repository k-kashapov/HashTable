#include <stdint.h>

int64_t SumHash      (const void *data_ptr, int len);
int64_t FirstSymHash (const void *data_ptr, int len);
int64_t StrLenHash   (const void *data_ptr, int len);
int64_t RoLHash      (const void *data_ptr, int len);
int64_t RoRHash      (const void *data_ptr, int len);
int64_t MurmurHash2A (const void *data_ptr, int len);