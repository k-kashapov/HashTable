#include "HashTable.h"

static const int  InitTableCap = 8;
static const int  POISON       = 0x42;

int CreateTable (Hash_t *target_table)
{
    TableElem *TableData = (TableElem *) calloc (InitTableCap, sizeof (TableElem));
    
    if (!TableData)
    {
        TABLE_ERR ("Unable to allocate memory of size: %d", 
                    InitTableCap);

        return MEM_ALLOC_ERR;
    }

    target_table->Data     = TableData;
    target_table->size     = 0;
    target_table->capacity = InitTableCap;

    return 0;
}

int64_t SimpleHash (const void *data_ptr, int len)
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

int TableInsert (Hash_t *target_table, const void *value, const void *key, int key_len)
{
    int64_t key_hash      = SimpleHash (key, key_len);
    int64_t capacity_mask = target_table->capacity - 1;

    void *target_list = target_table->Data[key_hash % capacity_mask].Data;

    if (!target_list)
    {
        List *new_list = (List *) calloc (1, sizeof (List));
        ListInit (new_list, 2);
        target_list = (void *) new_list;
    }
    
    type_t inserting = { key, value, key_len };

    ListPushBack ((List *) target_list, inserting);

    return 0;
}

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *))
{
    if (!target_table) return 0;

    for (int elem = 0; elem < target_table->capacity; elem++)
    {
        if (elem < target_table->size)
            elemDtor (target_table->Data[elem].Data);

        free (target_table->Data[elem].Data);

        target_table->Data[elem].Data = NULL;
    }

    target_table->size     = 0;
    target_table->capacity = 0;

    free (target_table->Data);

    return 0;
}
