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

    int created = CreateTable (&table, io_config.table_len);

    if (created)
    {
        DestrTable (&table, ListDtor);
        free_info (&src);
        return created;
    }

    type_t inserting = {};

    for (int word = 0; word < src.elems_num; word++)
    {
        inserting.key     = src.strs[word].text;
        inserting.key_len = src.strs[word].len;

        TableInsert (&table, inserting, FirstSymHash);
    }

    for (int i = 0; i < table.capacity; i++)
    {
        List *list = (List *) GetElemByHash (&table, i);

        type_t list_elem = GET_LIST_DATA (list, list->head);

        printf ("%.*s; %ld; of size; %d\n",
                list_elem.key_len, (const char *) list_elem.key, list->size, list_elem.key_len);
    }

    const char *search = "the";

    type_t elem_found = TableFind (&table, search, 3, FirstSymHash);

    printf ("looking for: |%s|\n"
            "found: |%.*s|\n"
            "key rep = %d\n",
            search, elem_found.key_len, (const char *) elem_found.key, elem_found.key_rep);

    DestrTable (&table, ListDtor);

    free_info (&src);

    return 0;
}
