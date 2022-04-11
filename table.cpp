#include "HashTable.h"

int main (int argc, const char **argv)
{
    

    Hash_t table = {};

    CreateTable (&table);

    type_t JOJO = { "jojo", "dio", 3 };

    for (int i = 0; i < table.capacity; i++)
    {
        void *elem = GetElemByHash (&table, i);

        printf ("%ld", ((List *) elem)->size);
    }

    DestrTable (&table, ListDtor);

    return 0;
}
