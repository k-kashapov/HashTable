#include "HashTable.h"
#include "string.h"

int CeilPowerOfTwo (int value)
{
    int res = 1;

    while (value - 1)
    {
        res   <<= 1;
        value >>= 1;
    }

    return res;
}

int CreateTable (Hash_t *target_table, int InitTableCap)
{
    if (InitTableCap < 2) InitTableCap = 2;
    else InitTableCap = CeilPowerOfTwo (InitTableCap);

    void **TableData = (void **) calloc ((size_t) InitTableCap, sizeof (void **));

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

    #ifdef LOGGING

    if (!LogFile)
    {
        fprintf (stderr, "ERROR: could not open Log File: |%s|!\n", LogPath);
        return OPEN_FILE_FAIL;
    }
    else
    {
        TABLE_MSG ("Table Created at <%p>", target_table);
    }

    #endif

    return 0;
}

// Relies highly on table capacity being a
// power of 2, so avoid changing it

int TableInsert (Hash_t *target_table, type_t value, HFunc_t UserHash)
{
    int64_t key_hash = UserHash (value.key, value.key_len);

    TABLE_MSG ("Adding element: |%.*s| of len = (%d)\n"
               "Hash = [%0x]\n",
               value.key_len, value.key, value.key_len, key_hash);

    List *target_list = (List *) GetElemByHash (target_table, key_hash);

    TABLE_MSG ("Found list: <%p> of size = (%d)\n", target_list, target_list->size);

    // Compare each element of list with the key we're looking for
    // If found, increment this element's counter and don't push

    long elem_number = ListFind (target_list, value);

    if (elem_number)
    {
        Node *list_elem = target_list->nodes + elem_number;
        list_elem->data.key_rep++;
        return 0;
    }

    // If not found, push new element to list

    TABLE_MSG ("NO element matched |%.*s|; Pushing new...",  value.key_len, value.key);

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

type_t TableFind (Hash_t *target_table, const void *key, int key_len, HFunc_t UserHash)
{
    if (key_len < 1)
    {
        TABLE_ERR ("Invalid key len! Expected > 0, got: %d\n", key_len);
        return {};
    }

    List *target_list = (List *) GetElemByHash (target_table, UserHash (key, key_len));

    if (target_list->size < 1) return {};

    type_t looking_for = { key, key, key_len };
    long   res_elem    = ListFind (target_list, looking_for);

    if (res_elem) return GET_LIST_DATA (target_list, res_elem);

    return {};
}

int TableDelete (Hash_t *target_table, const void *key, int key_len, HFunc_t UserHash)
{
    int64_t key_hash = UserHash (key, key_len);

    List *list = (List *) GetElemByHash (target_table, key_hash);

    type_t deleting = { key, key, key_len };

    long target_elem = ListFind (list, deleting);

    LOG_MSG ("found target_elem = (%d)\n", target_elem);

    if (target_elem)
    {
        ListPopPhys (list, target_elem);
    }
    else
    {
        LOG_MSG ("Element for deletion not found: |%.*s|!\n", key_len, key);
    }

    return 0;
}

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *))
{
    if (!target_table) return 0;

    #ifdef TABLE_LOGS
    if (LogFile) fclose (LogFile);
    #endif

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
