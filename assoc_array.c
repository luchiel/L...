//#define NDEBUG
#include <assert.h>
#include "linear_sequence_assoc.h"

typedef
    struct ArrayItemTag
    {
        struct ArrayItemTag * left;
        struct ArrayItemTag * right;
        struct ArrayItemTag * parent;
        LSQ_IntegerIndexT key;
        LSQ_BaseTypeT value;
        int height;
    }
        ArrayItemT, * ArrayItemPointerT;

typedef
    struct
    {
        ArrayItemPointerT root;
        ArrayItemPointerT bfirst;
        ArrayItemPointerT prear;
        LSQ_IntegerIndexT count;
    }
        AssocArrayT, * AssocArrayPointerT;

typedef
    struct
    {
        AssocArrayPointerT container;
        ArrayItemPointerT item;	
    }
        ArrayIteratorT, * ArrayIteratorPointerT;

static LSQ_IteratorT createIterator(LSQ_HandleT handle, ArrayItemPointerT item)
{
    ArrayIteratorPointerT it = NULL;
    if(handle == NULL || item == NULL) return LSQ_HandleInvalid;
    it = (ArrayIteratorPointerT)malloc(sizeof(ArrayIteratorT));
    if(it == NULL) return LSQ_HandleInvalid;	
    it->container = (AssocArrayPointerT)handle;
    it->item = item;
    return it;	
}

//повороты, балансировка
//передвижение по дереву - вынести... Через rewind и advance? 

LSQ_HandleT LSQ_CreateSequence(void)
{
    AssocArrayPointerT h = NULL;	

    h = (AssocArrayPointerT)malloc(sizeof(AssocArrayT));
    if(h == NULL) return LSQ_HandleInvalid;	
    h->bfirst = (ArrayItemPointerT)malloc(sizeof(ArrayItemPointerT));
    if(h->bfirst == NULL) return LSQ_HandleInvalid;
    h->prear = (ArrayItemPointerT)malloc(sizeof(ArrayItemPointerT));
    if(h->prear == NULL) return LSQ_HandleInvalid;

    h->root = NULL;
    h->count = 0;	

    h->bfirst->height = 0;
    h->bfirst->key = 0;
    h->bfirst->left = NULL;
    h->bfirst->right = NULL;
    h->bfirst->parent = h->root;

    h->prear->height = 0;
    h->prear->key = 0;
    h->prear->left = NULL;
    h->prear->right = NULL;
    h->prear->parent = h->root;

    return h;
}

void LSQ_DestroySequence(LSQ_HandleT handle)
{
    AssocArrayPointerT h = NULL;

    h = (AssocArrayPointerT)handle;
    if(h->root->left != NULL)
    {
        h->root; 
    }

    h = (AssocArrayPointerT)handle;
    if(h->root->right != NULL)
    {
    }

    free(handle);
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
    return handle == NULL ? -1 : ((AssocArrayPointerT)handle)->count;
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{	
    return
    !(iterator == NULL) &&
    !LSQ_IsIteratorBeforeFirst(iterator) &&
    !LSQ_IsIteratorPastRear(iterator);
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
    if(it == NULL) return 0;
    return it->item == it->container->prear;
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
    if(it == NULL) return 0;
    return it->item == it->container->bfirst;
}

LSQ_BaseTypeT * LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
    return !LSQ_IsIteratorDereferencable(it) ? LSQ_HandleInvalid : &(it->item->value);	
}

LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator)
{
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
    return !LSQ_IsIteratorDereferencable(it) ? 0 : it->item->key;	
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
    ArrayIteratorPointerT it = NULL;
    AssocArrayPointerT h = (AssocArrayPointerT)handle;
    if(h == NULL) return LSQ_HandleInvalid;
    it = createIterator(handle, h->root);	
    while(LSQ_IsIteratorDereferencable(it) && it->item->key != index)
    {
        if(it->item->key > index)
        {
            it->item = it->item->left;
        }
        else
        {
            it->item = it->item->right;
        }
    }
    if(!LSQ_IsIteratorDereferencable(it))
    {
        LSQ_DestroyIterator(it);
        it = LSQ_GetPastRearElement(handle);
    }
    return it;
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
    AssocArrayPointerT h = (AssocArrayPointerT)handle;
    ArrayIteratorPointerT it = createIterator(handle, h->bfirst);
    LSQ_AdvanceOneElement(it);
    return it;
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
    AssocArrayPointerT h = (AssocArrayPointerT)handle;
    return createIterator(handle, h->prear);
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
    free(iterator);
}

void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{	
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
    if(it == NULL) return;
    assert(it->item != NULL);
    if(it->item->right == NULL)
    {		
        if(it->item->parent == NULL || it->item->parent->left != it->item)
        {
            it->item = it->container->prear;
        }
        else
        {
            it->item = it->item->parent;
        }	
    }
    else
    {
        it->item = it->item->right;
        while(it->item->left != NULL)
        {
            it->item = it->item->left;
        }
    }
}

void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
}

void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
}

void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
    ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
}

void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value)
{
    ArrayItemPointerT i = NULL;
    AssocArrayPointerT h = (AssocArrayPointerT)handle;
    if(h == NULL) return;
    //h->item == NULL?
    i = h->root;
    while(i != NULL && i->key != key)
    {
        if(i->key > key)
        {
            i = i->left;
        }
        else
        {
            i = i->right;
        }
    }
    if(i == NULL)
    {
        //malloc, attach to prev
    }
    else
    {
        i->value = value;
    }
    //ballance!
}

void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
    //ballance!
}

void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
    //ballance!
}

void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key)
{
    //ballance!
}