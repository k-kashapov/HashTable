#include "HashTable.h"

int main (int argc, const char **argv)
{
    Hash_t table = {};

    CreateTable (&table);

    TableInsert (&table, (const void *) "jojo", (const void *) "dio", 3);

    DestrTable (&table, ListDtor);
    
    return 0;
}
