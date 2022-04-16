#include "HashTable.h"
#include "files.h"
#include "Hashing.h"

extern "C" int64_t MurmurHash (const void *data_ptr, int len);

#define HASH_USED MurmurHash

int StressTest (Hash_t *table, config io_config)
{
    file_info src = {};

    int read = read_all_words (&src, io_config.input_file);

    if (!read)
    {
        printf ("ERROR: Reading file failed!\n");
        return READ_TEXT_FAILED;
    }

    type_t inserting = {};

    for (int word = 0; word < src.elems_num; word++)
    {
        inserting.key     = src.strs[word].text;
        inserting.key_len = src.strs[word].len;

        TableInsert (table, inserting, HASH_USED);
    }

    // for (int i = 0; i < table->capacity; i++)
    // {
        // List *list = (List *) GetElemByHash (table, i);
        // printf ("%d; %d\n", i, list->size);
    // }

    for (int i = 0; i < 512; i++)
    {
        for (int word = 0; word < src.elems_num; word++)
        {
            volatile type_t find_res = TableFind (table, src.strs[word].text, src.strs[word].len, HASH_USED);
        }
    }

    for (int word = 0; word < src.elems_num; word++)
    {
        // printf ("deleting |%.*s|\n", src.strs[word].len, src.strs[word].text);

        TableDelete (table, src.strs[word].text, src.strs[word].len, HASH_USED);
    }

    DestrTable (table, ListDtor);

    free_info (&src);

    return 0;
}

int main (int argc, const char **argv)
{
    config io_config = {};
    get_params (argc, argv, &io_config);

    Hash_t table = {};

    int created = CreateTable (&table, io_config.table_len);

    if (created)
    {
        DestrTable (&table, ListDtor);
        return created;
    }

    StressTest (&table, io_config);

    return 0;
}
