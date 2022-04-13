#include "HashTable.h"
#include "files.h"
#include "Hashing.h"

int main (int argc, const char **argv)
{
    config io_config = {};
    file_info src = {};

    get_params (argc, argv, &io_config);

    int read = read_all_words (&src, io_config.input_file);

    if (!read)
    {
        printf ("ERROR: Reading file failed!\n");
        return READ_TEXT_FAILED;
    }

    Hash_t table = {};

    CreateTable (&table, io_config.table_len);

    type_t inserting = {};

    for (int word = 0; word < src.elems_num; word++)
    {
        inserting.key     = src.strs[word].text;
        inserting.key_len = src.strs[word].len;

        TableInsert (&table, inserting, RoLHash);
    }

    for (int i = 0; i < table.capacity; i++)
    {
        List *list = (List *) GetElemByHash (&table, i);

        type_t list_elem = GET_LIST_DATA (list, list->head);

        printf ("%.*s; %ld\n", list_elem.key_len, (char *) list_elem.key, list->size);
    }

    DestrTable (&table, ListDtor);

    free_info (&src);

    return 0;
}
