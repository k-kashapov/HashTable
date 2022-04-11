/*****************************************************************//**
 * \file   files.cpp
 * \brief  В файле собраны все функции, связанные с работой с файловой системой
 *********************************************************************/
#include "files.h"

int read_all_words (file_info *info, const char* file_name)
{
    assert (info);
    assert (file_name);

    FILE *source = NULL;
    if (open_file (&source, file_name, "rt"))
    {
        return OPEN_FILE_FAILED;
    }

    char *text_buff = read_to_end (source);

    if (text_buff == NULL)
    {
        return READ_TEXT_FAILED;
    }

    fclose (source);

    int words_num = CountWords (text_buff);

    string *strings = (string *) calloc (words_num, sizeof (string));
    assert (strings);

    string *strings_ptr = strings;
    
    for (char *token = strtok (text_buff, " \n,.:\r"); token;
         token = strtok (NULL, " \n,.:\r"))
    {
        fflush (stdout);
        
        if (*token != '\n')
        {           
            while (!isalpha (*token)) token++;
            
            char *token_ptr = token;

            while (isalpha (*token_ptr) && *token_ptr) token_ptr++;

            fflush (stdout);

            strings_ptr->len = token_ptr - token;
            (strings_ptr++)->text = token;

            fflush (stdout);
        } 
    } 
    
    fflush (stdout);
    
    info->text      = text_buff;
    info->strs      = strings;
    info->lines_num = strings_ptr - strings;
    
    return 0;
}

int CountWords (const char *text)
{
    int words_num = 0;
    
    for (int wordLen = 0; *text != '\0'; text++)
    {
        if (!isalpha (*text) && wordLen > 0)
        {
            wordLen = 0;
            continue;
        }
    
        if (isalpha (*text) && wordLen == 0)
        {
            words_num++;
        }
        
        wordLen += isalpha (*text);
    }

    return words_num;
}

int open_file (FILE **ptr, const char* file_name, const char* mode)
{
    *ptr = fopen (file_name, mode);
    if (!ptr)
    {
        printf ("ERROR: Couldn't open file \"%s\"\n", file_name);
        return (OPEN_FILE_FAILED);
    }

    return 0;
}

char* read_to_end (FILE *source) 
{
    assert (source);
    
    int length = get_len (source);

    char *text_buff = (char *) calloc ( length + 1, sizeof ( char ) );
    assert (text_buff);

    int sym_read = fread (text_buff, sizeof (*text_buff), length, source);
    
    if (sym_read < 0 || sym_read > length)
    {
         free (text_buff);
         printf ("ERROR: Reading text file failed");
         return (NULL);
    }

    // Останавливает дальнейшее чтение, т.к. дальше лежит мусор
    text_buff[sym_read] = '\0';
    
    return text_buff;
}

int get_len (FILE *file)
{
    assert (file);

    fseek (file, 0, SEEK_END);
    int length = ftell (file); 
    fseek (file, 0, SEEK_SET);

    return length;
}

void free_info (file_info *info)
{
    assert (info);

    free (info->text);
    free (info->strs);
}

void get_params (int argc, char **argv, config *current)
{
    assert (argv);
    assert (current);
    
    while (--argc)
    {
        char* arg = *++argv;
        if (!strncmp (arg, "-n", 2))
        {
            current->input_file = *++argv;
            argc--;
        }
        else 
        {
            if (!strncmp (arg, "-o", 2))
            {
                current->output_file = *++argv;
                argc--;
            }
            else 
            {
                printf ("Invalid paramter: %s\n", *argv);
            }
        }
    }
}
