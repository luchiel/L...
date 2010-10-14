#include <string.h>
#include "linear_sequence.h"

#define IteratorInvalid NULL

typedef
	enum
	{
		IT_DEREFERENCABLE,
		IT_BEFOREFIRST,
		IT_PASTREAR,
	}
		IteratorTypeT;
	
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

static LSQ_IteratorT createIterator(LSQ_HandleT handle);
static void reallocElements(LSQ_HandleT handle);
static void reallocIfOverflow(LSQ_HandleT handle);
static void reallocIfFreeSpace(LSQ_HandleT handle);

static LSQ_IteratorT createIterator(LSQ_HandleT handle)
{
	ArrayIteratorPointerT iterator;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	iterator = (ArrayIteratorPointerT)malloc(sizeof(ArrayIteratorT));
	if(iterator == IteratorInvalid) return IteratorInvalid;
	iterator->container = handle;
	return iterator;
}

static void reallocElements(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->size != 0)
		h->elements = realloc(h->elements, sizeof(LSQ_BaseTypeT) * h->size);	
}

static void reallocIfOverflow(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->count == h->size)
	{
		h->size++;
		reallocElements(handle);
	}
}

static void reallocIfFreeSpace(LSQ_HandleT handle)
{
	ArrayPointerT h = (ArrayPointerT)handle;
	if(h->count < h->size)
	{
		h->size = h->count;
		reallocElements(handle);
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
	if(handle == LSQ_HandleInvalid)
		return -1;
	else
		return ((ArrayPointerT)handle)->count;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	if(iterator == IteratorInvalid) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == IT_DEREFERENCABLE;
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	if(iterator == IteratorInvalid) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == IT_PASTREAR;
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	if(iterator == IteratorInvalid) return -1;
	return ((ArrayIteratorPointerT)iterator)->type == IT_BEFOREFIRST;
}

extern LSQ_BaseTypeT * LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	if(it == IteratorInvalid) return IteratorInvalid;
	return it->container->elements + it->index;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ArrayIteratorPointerT iterator = createIterator(handle);
	if(iterator == IteratorInvalid) return IteratorInvalid;
	iterator->index = index;
	if(index < 0)
	{
		iterator->type = IT_BEFOREFIRST;
		index = -1;
	}
	else
	{
		if(index >= ((ArrayPointerT)handle)->count)
		{
			iterator->type = IT_PASTREAR;
			index = ((ArrayPointerT)handle)->count;
		}
		else
		{
			iterator->type = IT_DEREFERENCABLE;
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
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	return LSQ_GetElementByIndex(handle, ((ArrayPointerT)handle)->count);	
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	if(iterator != IteratorInvalid)
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
	if(it == IteratorInvalid) return;
	it->index += shift;
	if(it->index < 0)
	{
		it->type = IT_BEFOREFIRST;
		it->index = -1;
	}
	else
	{
		if(it->index >= it->container->count)
		{
			it->type = IT_PASTREAR;
			it->index = it->container->count;
		}
		else
		{
			it->type = IT_DEREFERENCABLE;
		}
	}
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
	if(iterator == IteratorInvalid) return;
	((ArrayIteratorPointerT)iterator)->index = 0;
	LSQ_ShiftPosition(iterator, pos);
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ArrayIteratorPointerT it = LSQ_GetFrontElement(handle);
	LSQ_InsertElementBeforeGiven(it, element);	
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ArrayIteratorPointerT it = LSQ_GetPastRearElement(handle);
	LSQ_InsertElementBeforeGiven(it, element);	
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement)
{
	LSQ_BaseTypeT * placeOfElement = NULL;
	ArrayIteratorPointerT it = ((ArrayIteratorPointerT)iterator);
	if(it == IteratorInvalid || it->container == LSQ_HandleInvalid) return;
	reallocIfOverflow(it->container);	
	placeOfElement = it->container->elements + it->index;
	memmove(placeOfElement + 1, placeOfElement, sizeof(LSQ_BaseTypeT) * (it->container->count - it->index));	
	* placeOfElement = newElement;
	it->container->count++;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
	ArrayIteratorPointerT it = LSQ_GetFrontElement(handle);
	LSQ_DeleteGivenElement(it);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
	ArrayIteratorPointerT it = LSQ_GetPastRearElement(handle);
	LSQ_RewindOneElement(it);
	LSQ_DeleteGivenElement(it);	
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator)
{		
	LSQ_BaseTypeT * placeOfElement;
	ArrayIteratorPointerT it = ((ArrayIteratorPointerT)iterator);
	if(it == IteratorInvalid || it->type != IT_DEREFERENCABLE) return;
	if(it->container == LSQ_HandleInvalid) return;
	placeOfElement = it->container->elements + it->index;	
	if (it->container->count > 0)
	{
		memmove(placeOfElement, placeOfElement + 1, sizeof(LSQ_BaseTypeT) * (it->container->count - it->index - 1));
		it->container->count--;
		reallocIfFreeSpace(it->container);
    }	
}