#define LOG_NAME "HashTable"
#include "List.h"

#define TABLE_MSG(msg, ...) LOG_MSG ("HASH_TABLE:\n" msg, __VA_ARGS__);
#define TABLE_ERR(msg, ...) LOG_ERR ("HASH_TABLE:\n" msg, __VA_ARGS__);

struct Hash_t
{
    void **Data;
    int  size;
    int  capacity;
};

int CeilPowerOfTwo (int value);

int CreateTable (Hash_t *target_table, int InitTableCap);

int TableInsert (Hash_t *target_table, type_t value, int64_t (*UserHash) (const void *key, int len));

void *GetElemByHash (Hash_t *target_table, int64_t hash);

type_t TableFind (Hash_t *target_table, const void *key, int key_len, int64_t (* UserHash) (const void *, int));

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *));
