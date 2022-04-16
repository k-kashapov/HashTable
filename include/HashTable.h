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

typedef int64_t (* HFunc_t) (const void *, int);

int CeilPowerOfTwo (int value);

int CreateTable (Hash_t *target_table, int InitTableCap);

int TableInsert (Hash_t *target_table, type_t value, HFunc_t UserHash);

void *GetElemByHash (Hash_t *target_table, int64_t hash);

inline type_t TableFind (Hash_t *target_table, const void *key, int key_len, HFunc_t UserHash)
{
    type_t found = {};
    if (key_len < 1)
    {
        TABLE_ERR ("Invalid key len! Expected > 0, got: %d\n", key_len);
    }
    else
    {
        List *target_list = (List *) GetElemByHash (target_table, UserHash (key, key_len));

        if (target_list->size > 1)
        {
            type_t looking_for = { key, key, key_len };
            long   res_elem    = ListFind (target_list, looking_for);

            if (res_elem) found =  GET_LIST_DATA (target_list, res_elem);
        }
    }

    return found;
}

int TableDelete (Hash_t *target_table, const void *key, int key_len, HFunc_t UserHash);

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *));
