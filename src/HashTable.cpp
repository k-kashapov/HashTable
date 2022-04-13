#include "HashTable.h"
#include "string.h"

#ifdef TABLE_LOGS
    static FILE *LogFile = NULL
    #define TABLE_MSG(msg, ...) LogMsg (LogFile, 1, msg, __FUNCTION__, __LINE__, __VA_ARGS__);
#else
    #define LogFile stderr
    #define TABLE_MSG(msg, ...) ;
#endif

#define TABLE_ERR(msg, ...) LogMsg (LogFile, 1, msg, __FUNCTION__, __LINE__, __VA_ARGS__);

int CeilPowerOfTwo (int value)
{
    int res = 1;
    do
    {
        res   <<= 1;
        value >>= 1;
    } while (value - 1);

    return res;
}

int CreateTable (Hash_t *target_table, int InitTableCap)
{
    if (InitTableCap < 2) InitTableCap = 2;
    else InitTableCap = CeilPowerOfTwo (InitTableCap);

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

// Relies highly on table capacity being a
// power of 2, so avoid changing it

int TableInsert (Hash_t *target_table, type_t value, int64_t (*UserHash) (const void *key, int len))
{
    List *target_list = (List *) GetElemByHash (target_table, UserHash (value.key, value.key_len));

    // Compare each element of list with the key we're looking for
    // If found, increment this element's counter and don't push

    for (Node list_elem = GET_LIST_NODE (target_list, target_list->head);
         list_elem.next != 0;
         list_elem = GET_LIST_NODE (target_list, list_elem.next))
    {
        if (!strncmp ((const char *) list_elem.data.key,
                      (const char *) value.key,
                      value.key_len))
        {
            list_elem.data.key_rep += 1;
            return 0;
        }
    }

    // If not found, push new element to list

    ListPushBack (target_list, value);
    target_table->size++;

    return 0;
}

void *GetElemByHash (Hash_t *target_table, int64_t hash)
{
    int64_t capacity_mask = target_table->capacity - 1;

    void *target_elem = target_table->Data[hash & capacity_mask];

    return target_elem;
}

type_t TableFind (Hash_t *target_table, const void *key, int key_len, int64_t (* UserHash) (const void *, int))
{
    List *target_list = (List *) GetElemByHash (target_table, UserHash (key, key_len));

    for (Node curr_node = GET_LIST_NODE (target_list, target_list->head);
         curr_node.next != 0;
         curr_node = GET_LIST_NODE (target_list, curr_node.next))
    {
        int wanted = !strncmp ((const char *) curr_node.data.key, (const char *) key, key_len);

        if (wanted) return curr_node.data;
    }

    return GET_LIST_DATA (target_list, 0);
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
