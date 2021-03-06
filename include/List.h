#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOGS_NAME "LIST"

#include "Logs.h"
#define GET_LIST_NODE(lst, idx) ((*lst).nodes[idx])
#define GET_LIST_DATA(lst, idx) ((*lst).nodes[idx].data)

struct type_t
{
    const void *Data;
    const void *key;
    int        key_len;
    int        key_rep = 1; // Number of repetitions of key in hash table
};

#define _type_name "int64_t"

const long LIST_INIT_CAP = 2;

enum EXIT_CODES
{
    OK                =  0,
    MEM_ALLOC_ERR     = -1,
    INVALID_INS_ARG   = -2,
    OPEN_FILE_FAIL    = -3,
    POP_FIND_ERR      = -4,
};

#define LST_ERR(name, code) name = code,

enum LIST_STATES
{
    #include "lst_errs.h"
};
#undef LST_ERR

#ifdef LIST_LOGS
    #define LOG_PRINT(string) fprintf (Log_file, string);                       \
    fflush (Log_file);
    
    #define HLINE(width, height)    "<div><style scoped>"                       \
                                        "hr#w" #width                           \
                                        "{"                                     \
                                        "display: inline-block;"                \
                                        "position: relative;"                   \
                                        "margin-top: -"#height"px;"             \
                                        "margin-left: -240px;"                  \
                                        "border-style: inset;"                  \
                                        "border-width: 1px;"                    \
                                        "width: " #width "px;"                  \
                                        "}"                                     \
                                    "</style>"                                  \
                                    "<hr id = \"w" #width "\"></div>"

    #define LOG_FATAL(string) fprintf (stderr,  "\n\n<<img src = \"src/fatal.jpg\""      \
                                                "width = 150px>\nem style = \"color:"   \
                                                "red\">FATAL: " string "</em>")

    #define LOG_ERROR(string, ...)  fprintf (Log_file,  "\n%s: \n<img src = \"src/error.jpg\" width = 150px>\n"   \
                                                        "<em style = \"color : red\">ERROR: </em>"              \
                                                        #string "\n" HLINE (1000, 0), __FUNCTION__ __VA_ARGS__) \

    #define LIST_OK() ;
#else
    #define LIST_OK() ;
    #define HLINE(width, height)
    #define LOG_ERROR(string, ...) fprintf (stderr, "%s: ERROR: " #string "\n", __FUNCTION__ __VA_ARGS__)
    #define LOG_FATAL(string) fprintf (stderr, "FATAL: " string)
    #define LOG_PRINT(string) ;
#endif


#define REALLOC(arr, type, size)                                                \
    {                                                                           \
        type *tmp_ptr = (type *) realloc (arr, sizeof (type) * (size_t)size);   \
        if (!tmp_ptr)                                                           \
        {                                                                       \
            LOG_FATAL ("ALLOCATING MEMORY FAIL\n");                             \
            return MEM_ALLOC_ERR;                                               \
        }                                                                       \
        arr = tmp_ptr;                                                          \
    }

#define KILL_PTR(ptr, type)                                                     \
    if (ptr)                                                                    \
    {                                                                           \
        free (ptr);                                                             \
        ptr = (type *)POISON;                                                   \
    }

#define SWAP(var1, var2, type)                                                  \
    {                                                                           \
        type tmp = var1;                                                        \
        var1 = var2;                                                            \
        var2 = tmp;                                                             \
    }

struct Node
{
    type_t data;
    long next;
    long prev;
};

struct List
{
    Node *nodes;
    long head;
    long tail;
    long free;
    long capacity;
    long size;
    char linear;
};

int64_t ListInit (List *lst, long init_size = 1);

long LogicalToPhysicalAddr (List *lst, long num);

long ListFind (List *target_list, type_t value);

long ListGetHead (List *lst);

long ListGetTail (List *lst);

long ListLinearize (List *lst);

long ListInsertPhys (List *lst, type_t value, long place = -1);

long ListPushBack (List *lst, type_t value);

long ListPushFront (List *lst, type_t value);

type_t ListPopBack (List *lst, int *pop_err = NULL);

type_t ListPopFront (List *lst, int *pop_err = NULL);

type_t ListPopPhys (List *lst, long place, int *pop_err = NULL);

int64_t ListOK (List *lst);

int64_t ListResize (List *lst, long new_capacity);

int ListDtor (void *lst_void);

int64_t ListDump (List *lst, int64_t err, const char *called_from);

#endif