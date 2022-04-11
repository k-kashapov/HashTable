#include "HashTable.h"

static const int  InitTableCap = 8;
static const int  POISON       = 0x42;

int CreateTable (Hash_t *target_table)
{
    void **TableData = (void **) calloc (InitTableCap, sizeof (void **));

    if (!TableData)
    {
        TABLE_ERR ("Unable to allocate memory of size: %d",
                    InitTableCap);

        return MEM_ALLOC_ERR;
    }

    target_table->Data     = TableData;
    target_table->size     = 0;
    target_table->capacity = InitTableCap;

    for (int elem = 0; elem < InitTableCap; elem++)
    {
        List *new_list = (List *) calloc (1, sizeof (List));
        ListInit (new_list, 2);
        target_table->Data[elem] = new_list;
    }

    return 0;
}

int64_t SumHash (const void *data_ptr, int len)
{
    int64_t hash = 0;

    const char *data = (const char *) data_ptr;

    for (int byte = 0; byte < len; byte++)
    {
        hash += data[byte];
    }

    return hash;
}

// Relies highly on table capacity being a
// power of 2, so avoid changing it

int TableInsert (Hash_t *target_table, type_t value, int64_t (*UserHash) (const void *key, int len))
{
    int64_t key_hash      = UserHash (value.key, value.key_len);
    int64_t capacity_mask = target_table->capacity - 1;

    List *target_list = (List *) target_table->Data[key_hash % capacity_mask];

    if (target_list->size == 0)
    {
        target_table->size++;
    }

    ListPushBack (target_list, value);

    return 0;
}

void *GetElemByHash (Hash_t *target_table, int64_t hash)
{
    int64_t capacity_mask = target_table->capacity - 1;

    void *target_elem = target_table->Data[hash % capacity_mask];

    return target_elem;
}

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *))
{
    if (!target_table) return 0;

    for (int elem = 0; elem < target_table->capacity; elem++)
    {
        void *table_elem = target_table->Data[elem];

        elemDtor (table_elem);

        free (table_elem);

        table_elem = NULL;
    }

    target_table->size     = 0;
    target_table->capacity = 0;

    free (target_table->Data);

    return 0;
}
