#include "List.h"
#include <x86intrin.h>

#ifdef LIST_LOGS
    static int  OpenLogFile ();
    static int  CloseLogFile ();
    static FILE *Log_file   = NULL;
    static int  GRAPH_NUM   = 0;
#else
    static FILE *Log_file = stderr;
#endif

const int POISON = 0x42;

int64_t ListInit (List *lst, long init_size)
{
    #ifdef LIST_LOGS
        int open_err = OpenLogFile ();
        if (open_err) return OPEN_FILE_FAIL;
    #endif

    if (init_size < 1) init_size = 1;

    REALLOC (lst->nodes, Node, init_size);

    for (long elem = 0; elem < init_size; elem++)
    {
        lst->nodes[elem].data = {};
        lst->nodes[elem].next = (elem + 1) % init_size;
        lst->nodes[elem].prev = -1;
    }

    lst->nodes[0].next  = 0;
    lst->nodes[0].prev  = 0;
    lst->capacity       = init_size;
    lst->tail           = 0;
    lst->head           = 0;
    lst->size           = 0;
    lst->free           = 1;
    lst->linear         = 1;

    LOG_PRINT ("<em style = \"color : #16c95e\">List Initialized</em>\n");
    LIST_OK();

    return OK;
}

long ListFind (List *target_list, type_t value)
{
    long list_iterator = target_list->head;

    for (int steps = 0;
         steps < target_list->size;
         steps++,
         list_iterator = target_list->nodes[list_iterator].next)
    {
        Node *list_elem = target_list->nodes + list_iterator;

        LOG_MSG ("comparing value: |%.*s| (%d)\n"
                 "with       list: |%.*s| (%d)\n",
                 value.key_len,           value.key,           value.key_len,
                 list_elem->data.key_len, list_elem->data.key, list_elem->data.key_len);

        if (list_elem->data.key_len != value.key_len) continue;

        __m128i packed_list = _mm_lddqu_si128 ((__m128i *) list_elem->data.key);
        __m128i packed_val  = _mm_lddqu_si128 ((__m128i *) value.key);

        __m128i compared    = _mm_cmpeq_epi8 (packed_val, packed_list);

        // cmp_mask = { list[0] == value[0], list[1] == value[1], ... }

        int cmp_mask = _mm_movemask_epi8 (compared);

        // mask_for_mask = 0b00...0011...11;
        //                          <----->
        //                          key_len

        int mask_for_mask = (1 << value.key_len) - 1;

        LOG_MSG ("cmp mask = %08x\n"
                 "mask_for_mask = %08x\n"
                 "and: %08x\n"
                 "neq: %08x\n"
                 "not: %08x\n", 
                 cmp_mask, mask_for_mask, cmp_mask & mask_for_mask,
                 (~cmp_mask & mask_for_mask), !(~cmp_mask & mask_for_mask) );

        if (!(~cmp_mask & mask_for_mask))
        {
            LOG_MSG ("MATCHED! Key_rep = (%d)\n", list_elem->data.key_rep);
            return list_iterator;
        }

        LOG_MSG ("NOT MATCHED! Key = (%.4s)\n", list_elem->data.key);
    }

    return 0;
}

long LogicalToPhysicalAddr (List *lst, long num)
{
    LIST_OK ();
    if (lst->linear)
    {
        return num + lst->head - 1;
    }
    long elem  = lst->head;
    long steps = 1;
    for (; steps < num; elem = lst->nodes[elem].next, steps++) ;

    return elem;
}

long ListGetHead (List *lst)
{
    LIST_OK ();
    return lst->head;
}

long ListGetTail (List *lst)
{
    LIST_OK ();
    return lst->tail;
}

static long ListSwap (List *lst, long elem1, long elem2)
{
    lst->nodes[lst->nodes[elem1].prev].next = elem2;
    lst->nodes[lst->nodes[elem2].prev].next = elem1;

    lst->nodes[lst->nodes[elem1].next].prev = elem2;
    lst->nodes[lst->nodes[elem2].next].prev = elem1;

    SWAP (lst->nodes[elem1], lst->nodes[elem2], Node);

    return OK;
}

long ListLinearize (List *lst)
{
    LIST_OK ();

    long steps = 1;
    for (long elem = lst->head; lst->nodes[elem].next != 0; elem = lst->nodes[elem].next, steps++)
    {
        if (elem != steps)
        {
            ListSwap (lst, elem, steps);
            elem = steps;
        }
    }

    for (long elem = 1; elem < lst->size; elem++)
    {
        lst->nodes[elem].next = elem + 1;
        lst->nodes[elem].prev = elem - 1;
    }

    lst->nodes[0].data         = {};
    lst->nodes[0].next         = 0;
    lst->nodes[0].prev         = 0;
    lst->nodes[lst->size].next = 0;
    lst->nodes[lst->size].prev = lst->size - 1;

    lst->head   = 1;
    lst->tail   = lst->size;
    lst->free   = (lst->size + 1) % lst->capacity;
    lst->linear = 1;

    LIST_OK ();
    return OK;
}

long ListInsertPhys (List *lst, type_t value, long place)
{
    LIST_OK();
    if (place > lst->capacity)
    {
        LOG_ERROR (
            INVALID INSERT ARG\nplace = %ld > capacity = %ld!\n, ,
            place, lst->capacity
        );
        return INVALID_INS_ARG;
    }

    if (lst->tail == 0)
    {
        LOG_ERROR (INVALID INSERT\ninsert after 0 elem!\n);
        return INVALID_INS_ARG;
    }
    else if (place == -1 || place == lst->tail)
    {
        return ListPushBack (lst, value);
    }
    else if (place <= 0)
    {
        LOG_ERROR (INVALID INSERT ARG\nplace = %ld < 0!\n, , place);
        return INVALID_INS_ARG;
    }
    else if (lst->nodes[place].prev == -1)
    {
        LOG_ERROR (INVALID INSERT ARG\nno elem at place = %ld\n, , place);
        return INVALID_INS_ARG;
    }

    if (lst->free < 1)
    {
        ListResize (lst, lst->capacity * 2);
    }
    long dest = lst->free;

    lst->free              = lst->nodes[dest].next;
    lst->nodes[dest].data  = value;
    lst->nodes[dest].next  = lst->nodes[place].next;
    lst->nodes[place].next = dest;
    lst->nodes[dest].prev  = place;
    lst->linear            = 0;
    lst->size++;

    LIST_OK();

    return dest;
}

long ListPushBack (List *lst, type_t value)
{
    LIST_OK();

    if (lst->free < 1)
    {
        ListResize (lst, lst->capacity * 2);
    }
    long dest = lst->free;

    if (lst->tail == 0)
    {
        lst->free             = lst->nodes[dest].next;
        lst->tail             = dest;
        lst->head             = dest;

        lst->nodes[dest].data = value;
        lst->nodes[dest].next = 0;
        lst->nodes[dest].prev = 0;
        lst->size++;

        return dest;
    }

    lst->free                  = lst->nodes[dest].next;

    lst->nodes[dest].data      = value;
    lst->nodes[dest].next      = 0;
    lst->nodes[dest].prev      = lst->tail;

    lst->nodes[lst->tail].next = dest;
    lst->tail                  = dest;

    lst->size++;

    LIST_OK();
    return dest;
}

long ListPushFront (List *lst, type_t value)
{
    LIST_OK();
    if (lst->free < 1)
    {
        ListResize (lst, lst->capacity * 2);
    }
    long dest = lst->free;

    if (lst->tail == 0)
    {
        lst->free             = lst->nodes[dest].next;
        lst->tail             = dest;
        lst->head             = dest;
        lst->nodes[dest].data = value;
        lst->nodes[dest].next = 0;
        lst->nodes[dest].prev = 0;
        lst->size++;

        return dest;
    }

    lst->free                  = lst->nodes[lst->free].next;
    lst->nodes[dest].data      = value;
    lst->nodes[dest].next      = lst->head;
    lst->nodes[dest].prev      = 0;
    lst->nodes[lst->head].prev = dest;
    lst->head                  = dest;
    lst->linear                = 0;
    lst->size++;

    LIST_OK();
    return dest;
}

type_t ListPopBack (List *lst, int *pop_err)
{
    LIST_OK();

    if (lst->tail == 0)
    {
        LOG_ERROR (ZERO ELEMENT POP\n);
        if (pop_err) *pop_err = POP_FIND_ERR;
        return {};
    }

    if (lst->tail == lst->head)
    {
        type_t tmp                 = lst->nodes[lst->tail].data;
        lst->nodes[lst->tail].next = lst->free;
        lst->free                  = lst->tail;
        lst->nodes[lst->tail].prev = -1;
        lst->nodes[lst->tail].data = {};
        lst->tail                  = 0;
        lst->head                  = 0;
        lst->size--;

        return tmp;
    }

    long prev = lst->nodes[lst->tail].prev;
    if (prev == -1)
    {
        LOG_ERROR (NO ELEMENT FOUND: expected next = %ld</em>\n, , lst->tail);
        if (pop_err) *pop_err = POP_FIND_ERR;
        return {};
    }

    type_t tmp                 = lst->nodes[lst->tail].data;
    lst->nodes[prev].next      = 0;
    lst->nodes[lst->tail].next = lst->free;
    lst->free                  = lst->tail;
    lst->nodes[lst->tail].prev = -1;
    lst->nodes[lst->tail].data = {};
    lst->tail                  = prev;
    lst->size--;

    if (lst->size <= lst->capacity / 4)
    {
        ListResize (lst, lst->capacity / 2);
    }

    LIST_OK();

    return tmp;
}

type_t ListPopFront (List *lst, int *pop_err)
{
    LIST_OK();

    if (lst->head == 0)
    {
        LOG_ERROR (ZERO ELEMENT POP\n);
        if (pop_err) *pop_err = POP_FIND_ERR;
        return {};
    }

    if (lst->tail == lst->head)
    {
        type_t tmp                 = lst->nodes[lst->tail].data;
        lst->nodes[lst->tail].next = lst->free;
        lst->free                  = lst->tail;
        lst->nodes[lst->tail].prev = -1;
        lst->nodes[lst->tail].data = {};
        lst->tail                  = 0;
        lst->head                  = 0;
        lst->size--;

        return tmp;
    }

    long next = lst->nodes[lst->head].next;

    type_t tmp                 = lst->nodes[lst->head].data;
    lst->nodes[lst->head].data = {};
    lst->nodes[lst->head].next = lst->free;
    lst->free                  = lst->head;
    lst->nodes[lst->head].prev = -1;
    lst->head                  = next;
    lst->size--;

    if (lst->size <= lst->capacity / 4)
    {
        ListResize (lst, lst->capacity / 2);
    }

    LIST_OK();
    return tmp;
}

type_t ListPopPhys (List *lst, long place, int *pop_err)
{
    LIST_OK();

    if (lst->tail == 0)
    {
        LOG_ERROR (ZERO ELEMENT POP\n);
        if (pop_err) *pop_err = POP_FIND_ERR;
        return {};
    }

    if (place == lst->head)
    {
        return ListPopFront (lst, pop_err);
    }

    if (place == lst->tail)
    {
        return ListPopBack (lst, pop_err);
    }

    long prev = lst->nodes[place].prev;
    if (prev == -1)
    {
        LOG_ERROR (NO ELEMENT FOUND: expected next = %ld</em>\n, , lst->tail);
        if (pop_err) *pop_err = POP_FIND_ERR;
            return {};
    }

    type_t tmp = lst->nodes[place].data;

    lst->nodes[lst->nodes[place].prev].next = lst->nodes[place].next;
    lst->nodes[lst->nodes[place].next].prev = lst->nodes[place].prev;
    lst->nodes[place].next                  = lst->free;
    lst->free                               = place;
    lst->nodes[place].prev                  = -1;
    lst->nodes[place].data                  = {};
    lst->linear                             = 0;
    lst->size--;

    if (lst->size <= lst->capacity / 4)
    {
        ListResize (lst, lst->capacity / 2);
    }

    LIST_OK();
    return tmp;
}

int64_t ListOK (List *lst)
{
    int64_t err = 0;

    // Checking if all pointers are valid
    if (lst->nodes == NULL || lst->nodes == (Node *)POISON)
    {
        err |= BAD_PTR;
    }

    // Checking if prev linked to current and next
    // Also steps counter must be equal to size of the list
    if (err & BAD_PTR)
    {
        return err;
    }

    long steps = 2;
    long curr  = 0;
    for
    (
        curr = lst->nodes[lst->head].next;
        lst->nodes[curr].next != 0;
        curr = lst->nodes[curr].next, steps++
    )
    {
        if (lst->nodes[lst->nodes[curr].next].prev != curr)
        {
            err |= NEXT_NOT_LINKED;
            break;
        }
        if (lst->nodes[lst->nodes[curr].prev].next != curr)
        {
            err |= PREV_NOT_LINKED;
            break;
        }
        if (steps > lst->size)
        {
            err |= ENDLESS_LOOP;
            break;
        }
    }

    if (steps < lst->size && lst->nodes[curr].next == 0)
    {
        err |= UNEXPECTED_END;
    }

    // For each free prev must be -1; number of free's equals capacity - size
    steps         = 1;
    long free_num = lst->capacity - lst->size - 1;
    for (curr = lst->free; lst->nodes[curr].next != 0; curr = lst->nodes[curr].next, steps++)
    {
        if (lst->nodes[curr].prev != -1)
        {
            err |= INVALID_FREE_PREV;
            break;
        }
        if (steps > free_num)
        {
            err |= ENDLESS_FREE_LOOP;
            break;
        }
    }

    if (steps < free_num && lst->nodes[curr].next == 0)
    {
        err |= UNEXPECTED_FREE_END;
    }

    return err;
}

int64_t ListResize (List *lst, long new_capacity)
{
    LIST_OK();

    if (new_capacity <= 0) new_capacity = LIST_INIT_CAP;

    long buff_len = new_capacity * (long) sizeof (type_t);

    REALLOC (lst->nodes, Node, buff_len);

    for (long iter = lst->capacity; iter < new_capacity - 1; iter++)
    {
        lst->nodes[iter].data = {};
        lst->nodes[iter].next = iter + 1;
        lst->nodes[iter].prev = -1;
    }

    lst->nodes[new_capacity - 1].data = {};
    lst->nodes[new_capacity - 1].next = 0;
    lst->nodes[new_capacity - 1].prev = -1;
    lst->free = lst->size + 1;

    lst->capacity = new_capacity;

    LIST_OK();

    return OK;
}

int ListDtor (void *lst_void)
{
    List *lst = (List *) lst_void;

    LIST_OK();

    #ifdef LIST_LOGS
        CloseLogFile ();
    #endif

    KILL_PTR (lst->nodes, Node);

    return OK;
}

#ifdef LIST_LOGS
    #define LST_ERR(name, code)                                                 \
        if (err & code)                                                         \
        {                                                                       \
            LOG_ERROR (name\n);                                                 \
        }

    static int OpenLogFile ()
    {
        Log_file = fopen ("ListLog.html", "w");
        if (!Log_file)
        {
            printf ("OPENING LOG FILE FAILED\n");
            return OPEN_FILE_FAIL;
        }

        fprintf (Log_file, "<style>\
                            table, th, td\
                            {\
                                border:1px solid black;\
                                border-radius:6px;\
                                margin-left:100px;\
                                margin-right:auto;\
                                margin-top:1%%;\
                                table-layout:fixed;\
                                width:100px;\
                                text-align:center;\
                            }\
                            rect\
                            {\
                                position: relative;\
                                overflow: hidden;\
                                border:1px solid black;\
                                border-radius:6px;\
                                margin-left:100px;\
                                margin-top:10px;\
                                font-size:18px;\
                                width:80px;\
                                max-width:80px;\
                                text-align:center;\
                                display: inline-block;\
                            }\
                            </style>\
                            <body>\
                            <pre>");
        return OK;
    }

    static int CloseLogFile ()
    {
        if (Log_file)
        {
            fprintf (Log_file, "</pre></body>");
            fclose (Log_file);
            Log_file = NULL;
        }

        return OK;
    }

    static int64_t CreateListNode (FILE *output, long pos, Node node)
    {
        int64_t printed =  fprintf (output, "NODE%ld"
                                            "["
                                                "shape=record, style = \"rounded,filled\", "
                                                "fillcolor=\"%s\", "
                                                "label = \""
                                                "Data = |"
                                                "<pos>Position = %ld| "
                                                "{ <pr>Prev = %ld|"
                                                "<nx>Next = %ld }\""
                                            "]\n",
                                            pos, node.prev == -1 ? "pink" : "lightblue",
                                            pos, node.prev, node.next);
        return printed;
    }

    static FILE *OpenGraphFile (const char *name)
    {
        FILE *file_ptr = fopen (name, "w");
        if (!file_ptr)
        {
            printf ("OPENING GRAPH FILE FAILED\n");
            return NULL;
        }
        fprintf (file_ptr,  "digraph\n{\nrankdir = \"LR\";\n"
                            "splines = true;\n");
        return  file_ptr;
    }

    int64_t ListDump (List *lst, int64_t err, const char *called_from)
    {
        #ifdef LIST_LOGS
            if (!Log_file) return -1;

            FILE *Graph_file = OpenGraphFile ("dotInput.dot");
            FILE *LinearG_file = OpenGraphFile ("dotLinear.dot");

            const char *err_string = err ? "<em style = \"color : red\">ERROR</em>" :
                                           "<em style = \"color : #00FA9A\">ok</em>";

            fprintf (Log_file,  "[%s] [%s] List &#60%s&#62 [&%p] %s ; "
                                "called from %s\n",\
                                __DATE__, __TIME__, _type_name,
                                lst, err_string, called_from);

            #include "lst_errs.h"

            #ifdef MAX_INFO
                if (1)
            #else
                if (err)
            #endif
            {
                fprintf (Log_file,  "<pre>{\n"
                                        "\tcapacity = %ld;\n"
                                        "\tsize     = %ld;\n"
                                        "\tlinear   = %d",
                                    lst->capacity, lst->size, lst->linear);

                if (!lst->nodes)
                {
                    fprintf (Log_file, "\t}\n}\n </pre>");
                    return err;
                }

                CreateListNode (Graph_file, 0, lst->nodes[0]);
                CreateListNode (LinearG_file, 0, lst->nodes[0]);

                fprintf (Log_file, "<table> <tr> <th>Position</th>");

                for (long data_iter = 0; data_iter < lst->capacity; data_iter++)
                {
                    fprintf (Log_file, "<th>%4ld</th>", data_iter);
                    if (data_iter > 0)
                    {
                        CreateListNode (Graph_file, data_iter, lst->nodes[data_iter]);
                        CreateListNode (LinearG_file, data_iter, lst->nodes[data_iter]);

                        fprintf (Graph_file,    "NODE%ld->NODE%ld ["
                                                "len = 0.1, weight = 100, "
                                                "style = invis, "
                                                "constraint = true];\n",
                                                data_iter - 1, data_iter
                                );
                    }
                }

                fprintf (Log_file, "</tr> <tr><td>Data</td>");

                for (long data_iter = 0; data_iter < lst->capacity; data_iter++)
                {
                    if (data_iter > 0 && lst->nodes[data_iter].prev >= 0)
                    {
                        fprintf (Graph_file,    "NODE%ld:<nx>->NODE%ld:"
                                                "<pos> [color = blue, "
                                                "constraint = false];\n"
                                                "NODE%ld:<pr>->NODE%ld:"
                                                "<pos> [color = purple, "
                                                "constraint = false];\n",
                                                data_iter,
                                                lst->nodes[data_iter].next,
                                                data_iter,
                                                lst->nodes[data_iter].prev
                                );

                        fprintf (LinearG_file,  "NODE%ld:<nx>->NODE%ld:"
                                                "<pos> [color = blue, "
                                                "constraint = false];\n"
                                                "NODE%ld:<pr>->NODE%ld:"
                                                "<pos> [color = purple, "
                                                "constraint = false];\n",
                                                data_iter,
                                                lst->nodes[data_iter].next,
                                                data_iter,
                                                lst->nodes[data_iter].prev
                                );
                        fprintf (LinearG_file,  "NODE%ld->NODE%ld ["
                                                "len = 0.1, weight = 1000, "
                                                "style = invis, "
                                                "constraint = true];\n",
                                                data_iter, lst->nodes[data_iter].next
                                );
                    }

                }

                fprintf (Graph_file,    "INFO"
                                        "["
                                            "shape=record, style = \"rounded\","
                                            "label = \""
                                            "INFO|"
                                            "<hd>HEAD = %ld| "
                                            "<tl>TAIL = %ld|"
                                            "<fr>FREE = %ld\""
                                        "]\n",
                                        lst->head, lst->tail, lst->free
                        );

                fprintf (Graph_file,    "NODE%ld->INFO [len = 0.1, "
                                        "weight = 100, style = invis, "
                                        "constraint = true];\n", lst->capacity - 1
                        );

                fprintf (Graph_file,    "INFO:<hd>->NODE%ld "
                                        "[color = darkgreen, "
                                        "constraint = false];\n"

                                        "INFO:<tl>->NODE%ld "
                                        "[color = orange, "
                                        "constraint = false];\n"

                                        "INFO:<fr>->NODE%ld "
                                        "[color = red, "
                                        "constraint = false];\n",
                                        lst->head, lst->tail, lst->free
                        );

                fprintf (Graph_file, "\n}");
                fprintf (LinearG_file, "\n}");

                fclose (Graph_file);
                fclose (LinearG_file);

                fprintf (Log_file, "</tr> <tr> <td>Next</td>");

                for (long next_iter = 0; next_iter < lst->capacity; next_iter++)
                {
                    fprintf (Log_file, "<td>%4ld</td>", lst->nodes[next_iter].next);
                }

                fprintf (Log_file, "</tr> <tr> <td>Prev</td>");

                for (long prev_iter = 0; prev_iter < lst->capacity; prev_iter++)
                {
                    fprintf (Log_file, "<td>%4ld</td>", lst->nodes[prev_iter].prev);
                }

                fprintf (Log_file, "</tr></table>\n");

                fprintf (Log_file,  "<rect>Head\n%ld</rect>\n"
                                    "\n<rect>Tail\n%ld</rect>\n\n"
                                    "<rect>Free\n%ld</rect>\n\n"
                                    "\n}</pre>",
                                    lst->head, lst->tail, lst->free
                        );

                char sys_command[49] = {};
                snprintf (sys_command, 49,  "%s%d%s",
                                            "dot dotInput.dot -Tpng -o img/Img",
                                            GRAPH_NUM,
                                            ".png"
                        );

                system (sys_command);

                snprintf (sys_command, 49,  "%s%d%s",
                                            "dot dotLinear.dot -Tpng -o img/Lin",
                                            GRAPH_NUM,
                                            ".png"
                        );
                system (sys_command);

                fprintf (Log_file,  "\nPHYSICAL:\n<img src = \"img/Img%d.png\">\n"
                                    "\nLOGICAL :\n<img src = \"img/Lin%d.png\">\n",
                                    GRAPH_NUM, GRAPH_NUM
                        );
                GRAPH_NUM++;
                fprintf (Log_file, HLINE (1000, 0) "\n");
            }
        #endif

        return 0;
    }
#endif
