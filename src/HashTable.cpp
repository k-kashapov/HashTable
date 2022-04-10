#include "HashTable.h"

static const int InitTableLen = 4;

int CreateTable (Hash_t *target_table)
{
    TableElem *TableData = (TableElem *) calloc (InitTableLen, sizeof (TableElem));
    
    if (!TableData)
    {
        TABLE_ERR ("Unable to allocate memory of size: %d", 
                    InitTableLen);

        return MEM_ALLOC_ERR;
    }

    target_table->data     = TableData;
    target_table->size     = 0;
    target_table->capacity = 4;

    return 0;
}
