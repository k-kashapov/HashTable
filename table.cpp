#include "HashTable.h"
#include "files.h"

int main (int argc, const char **argv)
{
    file_info src = {};
    
    int read = read_all_words (&src, "Silmarillion.txt");

    Hash_t table = {};

    CreateTable (&table);

    type_t JOJO = { "jojo", "dio", 3 };

    for (int i = 0; i < table.capacity; i++)
    {
        void *elem = GetElemByHash (&table, i);

        printf ("%ld", ((List *) elem)->size);
    }

    DestrTable (&table, ListDtor);

    free_info (&src);

    return 0;
}
