//#define NDEBUG
#include <assert.h>
#include "linear_sequence_assoc_hash.h"
#include <stdlib.h>
#include <math.h>

#define BOX_COUNT 1000

typedef
    enum
    {
        IT_DEREFERENCABLE,
        IT_BEFORE_FIRST,
        IT_PAST_REAR,
    }
        IteratorTypeT;

typedef
	struct ItemTag
	{
        LSQ_KeyT key;
        LSQ_BaseTypeT value;
		struct ItemTag * next;
	}
		ItemT, * ItemPointerT;

typedef
    struct
    {
        LSQ_SizeT size;
        ItemPointerT * cabinet;

        LSQ_Callback_CloneFuncT * kcloneFunction;
        LSQ_Callback_SizeFuncT * ksizeFunction;
        LSQ_Callback_CompareFuncT * kcompareFunction;
        LSQ_Callback_CloneFuncT * vcloneFunction;
    }
        HashT, * HashPointerT;

typedef
    struct
    {
        IteratorTypeT type;
        HashPointerT hash;
        ItemPointerT item;
    }
        IteratorT, * IteratorPointerT;

static LSQ_IteratorT createIterator(LSQ_HandleT handle, IteratorTypeT type, ItemPointerT item);
static int calculateHash(LSQ_KeyT key, HashPointerT hash);

static LSQ_IteratorT createIterator(LSQ_HandleT handle, IteratorTypeT type, ItemPointerT item)
{
    IteratorPointerT it = NULL;
    if(handle == NULL)
        return LSQ_HandleInvalid;
    it = (IteratorPointerT)malloc(sizeof(IteratorT));;
    if(it == NULL)
        return LSQ_HandleInvalid;
    assert(type != IT_DEREFERENCABLE || item != NULL);
    it->hash = (HashPointerT)handle;
    it->type = type;
    it->item = item;
    return (LSQ_IteratorT)it;
}

static int calculateHash(LSQ_KeyT key, HashPointerT hash)
{
    int i = 0;
    int size = 0;
    unsigned int intKey = 0;
    double integer = 0;

    size = hash->ksizeFunction(key);
    
    while(i < size)
    {
        intKey += ((char *)key)[i] * (i + 1);
        i++;
    }
    return (int)((BOX_COUNT - 1) * modf(intKey * 0.49235786, &integer));
}

LSQ_HandleT LSQ_CreateSequence
(
    LSQ_Callback_CloneFuncT keyCloneFunc,
    LSQ_Callback_SizeFuncT keySizeFunc,
    LSQ_Callback_CompareFuncT keyCompFunc,
    LSQ_Callback_CloneFuncT valCloneFunc
)
{
    HashPointerT hash = (HashPointerT)malloc(sizeof(HashT));
    if(hash == NULL)
        return LSQ_HandleInvalid;
    
    hash->size = 0;
    hash->cabinet = (ItemPointerT *)calloc(BOX_COUNT, sizeof(ItemT));
    if(hash->cabinet == NULL)
    {
        free(hash);
        return LSQ_HandleInvalid;
    }

    assert(keyCloneFunc != NULL && keySizeFunc != NULL && keyCompFunc != NULL && valCloneFunc != NULL);
    hash->kcloneFunction = keyCloneFunc;
    hash->kcompareFunction = keyCompFunc;
    hash->ksizeFunction = keySizeFunc;
    hash->vcloneFunction = valCloneFunc;

    return (LSQ_HandleT)hash;
}

void LSQ_DestroySequence(LSQ_HandleT handle)
{
    int i;
    ItemPointerT item = NULL;
    ItemPointerT prev = NULL;
    HashPointerT hash = (HashPointerT)handle;
    if(hash == NULL)
        return;
    
    for(i = 0; i < BOX_COUNT; ++i)
    {
        item = hash->cabinet[i];
        while(item != NULL)
        {
            prev = item;
            item = item->next;
            free(prev->key);
            free(prev->value);
            free(prev);
        }
    }
    free(hash->cabinet);
    free(hash);
}

LSQ_SizeT LSQ_GetSize(LSQ_HandleT handle)
{
    HashPointerT hash = (HashPointerT)handle;
    if(hash == NULL)
        return 0;
    return hash->size;
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
    IteratorPointerT it = (IteratorPointerT)iterator;
    return it == NULL ? 0 : it->type == IT_DEREFERENCABLE;
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
    IteratorPointerT it = (IteratorPointerT)iterator;
    return it == NULL ? 0 : it->type == IT_PAST_REAR;
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
    IteratorPointerT it = (IteratorPointerT)iterator;
    return it == NULL ? 0 : it->type == IT_BEFORE_FIRST;
}

LSQ_BaseTypeT LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
    IteratorPointerT it = (IteratorPointerT)iterator;
    if(!LSQ_IsIteratorDereferencable(iterator))
        return LSQ_HandleInvalid;
    assert(it->item != NULL);
    return it->item->value;
}

LSQ_KeyT LSQ_GetIteratorKey(LSQ_IteratorT iterator)
{
    IteratorPointerT it = (IteratorPointerT)iterator;
    if(!LSQ_IsIteratorDereferencable(iterator))
        return LSQ_HandleInvalid;
    assert(it->item != NULL);
    return it->item->key;
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key)
{
    int box;
    ItemPointerT item = NULL;
    HashPointerT hash = (HashPointerT)handle;
    
    if(hash == NULL)
        return LSQ_HandleInvalid;

    box = calculateHash(key, hash);
    item = hash->cabinet[box];
    while(item != NULL && hash->kcompareFunction(item->key, key))
        item = item->next;
    
    return
        item == NULL ?
        LSQ_GetPastRearElement(handle) :
        createIterator(handle, IT_DEREFERENCABLE, item);
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
    int i = 0;
    HashPointerT hash = (HashPointerT)handle;
    if(hash == NULL)
        return LSQ_HandleInvalid;

    while(i < BOX_COUNT && hash->cabinet[i] == NULL)
        i++;
    
    return
        i == BOX_COUNT ?
        LSQ_GetPastRearElement(handle) :
        createIterator(handle, IT_DEREFERENCABLE, hash->cabinet[i]);
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
    return createIterator(handle, IT_PAST_REAR, NULL);
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
    free(iterator);
}

void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
    int box;
    IteratorPointerT it = (IteratorPointerT)iterator;
    if(it == NULL || !LSQ_IsIteratorDereferencable(iterator))
        return;
    assert(it->item != NULL);

    if(it->item->next != NULL)
    {
        it->item = it->item->next;
        return;
    }

    box = calculateHash(it->item->key, it->hash) + 1;
    while(box < BOX_COUNT && it->hash->cabinet[box] == NULL)
        box++;
    
    if(box == BOX_COUNT)
    {
        it->type = IT_PAST_REAR;
        it->item = NULL;
    }
    else
    {
        it->item = it->hash->cabinet[box];
    }
}

void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value)
{
    int box;
    ItemPointerT item = NULL;
    HashPointerT hash = (HashPointerT)handle;
    IteratorPointerT it = NULL;
    
    if(hash == NULL)
        return;
    
    it = (IteratorPointerT)LSQ_GetElementByIndex(handle, key);
    if(it != NULL && LSQ_IsIteratorDereferencable(it))
    {
        free(it->item->value);
        it->item->value = hash->vcloneFunction(value);
        LSQ_DestroyIterator(it);
        return;
    }
    LSQ_DestroyIterator(it);

    item = (ItemPointerT)malloc(sizeof(ItemT));
    if(item == NULL)
        return;

    box = calculateHash(key, hash);
    item->key = hash->kcloneFunction(key);
    item->value = hash->vcloneFunction(value);
    item->next = hash->cabinet[box];
    hash->cabinet[box] = item;
}

void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key)
{
    int box;
    ItemPointerT item = NULL;
    ItemPointerT prev = NULL;
    HashPointerT hash = (HashPointerT)handle;
    
    box = calculateHash(key, hash);
    item = hash->cabinet[box];
    while(item != NULL && hash->kcompareFunction(item->key, key))
    {
        prev = item;
        item = item->next;
    }

    if(item == NULL)
        return;

    if(prev == NULL)
        hash->cabinet[box] = item->next;
    else
        prev->next = item->next;

    free(item->key);
    free(item->value);
    free(item);

    hash->size--;
}