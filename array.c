//#include <stdlib.h>
#include <string.h>
#include "linear_sequence.h"

typedef
	enum {
		DEREFERENCABLE,
		BEFOREFIRST,
		PASTREAR
	} IteratorTypeT;
	
typedef
	struct
	{
		LSQ_BaseTypeT * elements;
		LSQ_IntegerIndexT count;	//логический размер
		LSQ_IntegerIndexT size;		//физический размер
	}
		ArrayT, * ArrayPointerT;
		
typedef
	struct
	{
		ArrayPointerT container;
		LSQ_IntegerIndexT index;		
		IteratorTypeT type;			
	}
		ArrayIteratorT, * ArrayIteratorPointerT;
	
static LSQ_IteratorT createIterator(LSQ_HandleT handle)
{
	ArrayIteratorPointerT iterator;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	iterator = (ArrayIteratorPointerT)malloc(sizeof(ArrayIteratorT));
	if(iterator == NULL) return NULL;
	iterator->container = handle;
	return iterator;
}

static void reallocElements(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->size != 0)
		h->elements = realloc(h->elements, sizeof(LSQ_BaseTypeT) * h->size);	
}

static void checkForOverflow(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->count == h->size)
	{
		h->size++;
		reallocElements(handle);
	}
}

static void checkForFreeSpace(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->count < h->size)
	{
		h->size = h->count;
		reallocElements(handle);
	}
}

static void insertElementAtIndex(LSQ_HandleT handle, LSQ_BaseTypeT element, LSQ_IntegerIndexT index)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	LSQ_BaseTypeT * placeOfElement;
	if(h == NULL) return;
	checkForOverflow(handle);	
	placeOfElement = h->elements + index;
	memmove(placeOfElement + 1, placeOfElement, sizeof(LSQ_BaseTypeT) * (h->count - index));	
	* placeOfElement = element;
	h->count++;
}

static void deleteElementAtIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ArrayPointerT h = (ArrayPointerT)handle;	
	LSQ_BaseTypeT * placeOfElement;
	if(h == NULL) return;
	placeOfElement = h->elements + index;
	if (h->count > 0)
	{
    	memmove(placeOfElement, placeOfElement + 1, sizeof(LSQ_BaseTypeT) * (h->count - index - 1));
    	h->count--;
    	checkForFreeSpace(handle);
    }
}

extern LSQ_HandleT LSQ_CreateSequence(void)
{
	ArrayPointerT handle = malloc(sizeof(ArrayT));
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	handle->elements = NULL;
	handle->count = 0;
	handle->size = 0;
	return handle; 
}

extern void LSQ_DestroySequence(LSQ_HandleT handle)
{
	if(handle != LSQ_HandleInvalid)
	{
		free(((ArrayPointerT)handle)->elements);
		free(handle);
	}
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{	
	if(handle == NULL)
		return -1;
	else
		return ((ArrayPointerT)handle)->count;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	if(iterator == NULL) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == DEREFERENCABLE;
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	if(iterator == NULL) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == PASTREAR;
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	if(iterator == NULL) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == BEFOREFIRST;
}

extern LSQ_BaseTypeT * LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	if(it == NULL) return NULL;
	return it->container->elements + it->index;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ArrayIteratorPointerT iterator = createIterator(handle);
	if(iterator == NULL) return NULL;
	iterator->index = index;
	if(index < 0)
	{
		iterator->type = BEFOREFIRST;
		index = -1;
	}
	else
	{
		if(index >= ((ArrayPointerT)handle)->count)
		{
			iterator->type = PASTREAR;
			index = ((ArrayPointerT)handle)->count;
		}
		else
		{
			iterator->type = DEREFERENCABLE;
		}
	}
	iterator->index = index;
	return iterator;
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	return LSQ_GetElementByIndex(handle, 0);
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
	ArrayIteratorPointerT iterator = createIterator(handle);
	if(iterator == NULL) return NULL;
	iterator->index = ((ArrayPointerT)handle)->count;
	iterator->type = PASTREAR;
	return iterator;
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	if(iterator != NULL)
		free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
	LSQ_ShiftPosition(iterator, 1);
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
	LSQ_ShiftPosition(iterator, -1);
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	if(it == NULL) return;
	it->index += shift;
	if(it->index < 0)
	{
		it->type = BEFOREFIRST;
		it->index = -1;
	}
	else
	{
		if(it->index >= it->container->count)
		{
			it->type = PASTREAR;
			it->index = it->container->count;
		}
		else
		{
			it->type = DEREFERENCABLE;
		}
	}
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
	if(iterator == NULL) return;
	((ArrayIteratorPointerT)iterator)->index = 0;
	LSQ_ShiftPosition(iterator, pos);
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	insertElementAtIndex(handle, element, 0);
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	if(handle == LSQ_HandleInvalid) return;
	insertElementAtIndex(handle, element, ((ArrayPointerT)handle)->count);
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement)
{
	ArrayIteratorPointerT it = ((ArrayIteratorPointerT)iterator);
	if(it == NULL) return;
	insertElementAtIndex(it->container, newElement, it->index);
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
	deleteElementAtIndex(handle, 0);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
	if(handle == LSQ_HandleInvalid) return;
	deleteElementAtIndex(handle, ((ArrayPointerT)handle)->count - 1);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator)
{	
	ArrayIteratorPointerT it = ((ArrayIteratorPointerT)iterator);
	if(it == NULL || it->type != DEREFERENCABLE) return;
	deleteElementAtIndex(it->container, it->index);
}