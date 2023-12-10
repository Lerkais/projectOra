#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"

void gfc_list_delete(gfcList *list)
{
    if (!list)return;
    if (list->elements)
    {
        free(list->elements);
    }
    free(list);
}

gfcList *gfc_list_new()
{
    return gfc_list_new_size(16);
}

gfcList *gfc_list_copy(gfcList *old)
{
    gfcList *new;
    if (!old)return 0;
    if (old->size <= 0)return NULL;
    new = gfc_list_new_size(old->size);
    if (!new)return NULL;
    if (old->count <= 0)return new;
    memcpy(new->elements,old->elements,sizeof(ListElementData)*old->count);
    new->count = old->count;
    return new;
}

gfcList *gfc_list_new_size(Uint32 count)
{
    gfcList *l;
    if (!count)
    {
        slog("cannot make a list of size zero");
        return NULL;
    }
    l = (gfcList *)malloc(sizeof(gfcList));
    if (!l)
    {
        slog("failed to allocate space for the list");
        return NULL;
    }
    memset(l,0,sizeof(gfcList));
    l->size = count;
    l->elements = gfc_allocate_array(sizeof(ListElementData),count);
    if (!l->elements)
    {
        slog("failed to allocate space for list elements");
        free(l);
        return NULL;
    }
    return l;
}

void gfc_list_set_nth(gfcList *list,Uint32 n,void *data)
{
    if (!list)return;
    if ((n >= list->count)||(n >= list->size))return;
    list->elements[n].data = data;
}

void gfc_list_swap_indices(gfcList *list,Uint32 a, Uint32 b)
{
    void *temp = NULL;
    if (!list)return;
    if (a == b)return;
    if ((a >= list->count)||(b >= list->count))return;
    if ((a >= list->size)||(b >= list->size))return;
    temp = list->elements[a].data;
    list->elements[a].data = list->elements[b].data;
    list->elements[b].data = temp;
}

void *gfc_list_get_nth(gfcList *list,Uint32 n)
{
    if (!list)
    {
        return NULL;
    }
    if ((n >= list->count)||(n >= list->size))return NULL;
    return list->elements[n].data;
}

gfcList *gfc_list_expand(gfcList *list)
{
    gfcList *l;
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if (!list->size)list->size = 8;
    l = gfc_list_new_size(list->size * 2);
    if (!l)
    {
        return list;
    }
    if (list->count > 0)
    {
        memcpy(l->elements,list->elements,sizeof(ListElementData)*list->count);
    }
    list->size = l->size;// update the new size
    free(list->elements);//free the old data
    list->elements = l->elements;//point to new memory address
    free(l);//free the temp list
    return list;//for backward compatibility
}

gfcList *gfc_list_append(gfcList *list,void *data)
{
    if (!list)
    {
        list = gfc_list_new();
    }
    if (list->count >= list->size)
    {
        list = gfc_list_expand(list);
        if (!list)
        {
            slog("append failed due to lack of memory");
            return NULL;
        }
    }
    list->elements[list->count].data = data;
    list->count++;
    return list;
}

gfcList *gfc_list_concat(gfcList *a,gfcList *b)
{
    int i,count;
    void *data;
    if ((!a) || (!b))
    {
        slog("missing list data");
        return NULL;
    }
    count = gfc_list_get_count(b);
    for (i = 0; i < count;i++)
    {
        data = gfc_list_get_nth(b,i);
        a = gfc_list_append(a,data);
        if (a == NULL)return NULL;
    }
    return a;
}

gfcList *gfc_list_concat_free(gfcList *a,gfcList *b)
{
    a = gfc_list_concat(a,b);
    if (a == NULL)return NULL;
    gfc_list_delete(b);
    return a;
}

gfcList *gfc_list_prepend(gfcList *list,void *data)
{
    return gfc_list_insert(list,data,0);
}

gfcList *gfc_list_insert(gfcList *list,void *data,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if (n > list->size + 1)
    {
        slog("attempting to insert element beyond length of list");
        return list;
    }
    if (list->count >= list->size)
    {
        list = gfc_list_expand(list);
        if (!list)return NULL;
    }
    memmove(&list->elements[n+1],&list->elements[n],sizeof(ListElementData)*(list->count - n));//copy all elements after n
    list->elements[n].data = data;
    list->count++;
    return list;
}


int gfc_list_delete_first(gfcList *list)
{
    return gfc_list_delete_nth(list,0);
}

int gfc_list_delete_last(gfcList *list)
{
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    return gfc_list_delete_nth(list,list->count-1);
}

int gfc_list_get_item_index(gfcList *list,void *data)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (!data)return 0;
    for (i = 0; i < list->count;i++)
    {
        if (list->elements[i].data == data)
        {
            // found it
            return i;
        }
    }
    return -1;    
}

int gfc_list_delete_data(gfcList *list,void *data)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (!data)return 0;
    for (i = 0; i < list->count;i++)
    {
        if (list->elements[i].data == data)
        {
            // found it, now delete it
            gfc_list_delete_nth(list,i);
            return 0;
        }
    }
    slog("data not found");
    return -1;
}

void gfc_list_clear(gfcList *list)
{
    if (!list)return;
    memset(list->elements,0,list->size);//zero out all the data;
    list->count = 0;
}

int gfc_list_delete_nth(gfcList *list,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (n >= list->count)
    {
        slog("attempting to delete beyond the length of the list");
        return -1;
    }
    if (n == (list->count - 1))
    {
        list->count--;// last element in the array, this is easy
        list->elements[n].data = NULL;
        return 0;
    }
    memmove(&list->elements[n],&list->elements[n+1],sizeof(ListElementData)*(list->count - n));//copy all elements after n
    list->count--;
    return 0;
}

Uint32 gfc_list_get_count(gfcList *list)
{
    if (!list)return 0;
    return list->count;
}

void gfc_list_foreach(gfcList *list,void (*function)(void *data))
{
    int i;
    if (!list)
    {
        return;
    }
    if (!function)
    {
        slog("no function provided");
    }
    for (i = 0;i < list->count;i++)
    {
        function(list->elements[i].data);
    }
}

void gfc_list_foreach_context(gfcList *list,void (*function)(void *data,void *context),void *contextData)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return;
    }
    if (!function)
    {
        slog("no function provided");
    }
    for (i = 0;i < list->count;i++)
    {
        function(list->elements[i].data,contextData);
    }
}

/*eol@eof*/
