#include "linear_sequence.h"

typedef		
	struct
	{
		LSQ_BaseTypeT * element;
		void * next;
		void * prev;
	}
		ListItemT, * ListItemPointerT;
		
typedef
	struct
	{
		ListItemPointerT container;
	}
		ListIteratorT, * ListIteratorPointerT;

static LSQ_IteratorT createIterator(LSQ_HandleT handle)
{
	ListIteratorPointerT iterator;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	iterator = (ListIteratorPointerT)malloc(sizeof(ListIteratorT));
	if(iterator == NULL) return NULL;
	iterator->container = handle;
	return iterator;
}

extern LSQ_HandleT LSQ_CreateSequence(void)
{
	ListItemPointerT bfirst;
	ListItemPointerT prear;	
	bfirst = (ListItemPointerT)malloc(sizeof(ListItemT));
	prear = (ListItemPointerT)malloc(sizeof(ListItemT));	
	if(bfirst == LSQ_HandleInvalid || prear == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	prear->prev = bfirst;
	prear->next = NULL;
	prear->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	bfirst->prev = NULL;
	bfirst->next = prear;	
	bfirst->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	return bfirst; 
}

extern void LSQ_DestroySequence(LSQ_HandleT handle)
{
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	if(it == NULL) return;
	while(it->container->next != LSQ_HandleInvalid)
	{
		free(it->container->prev);
		it->container = (ListItemPointerT)(it->container->next);
	}
	free(it->container);
	LSQ_DestroyIterator(it);
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
	int i;
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	if(it == NULL) return -1;
	i = 0;
	while(it->container->next != LSQ_HandleInvalid)
	{
		it->container = (ListItemPointerT)(it->container->next);
		i++;
	}
	LSQ_DestroyIterator(it);
	return i;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == NULL) return -1;
	return (it->container->prev != LSQ_HandleInvalid && it->container->next != LSQ_HandleInvalid);
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == NULL) return -1;
	return (it->container->next == LSQ_HandleInvalid);
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == NULL) return -1;
	return (it->container->prev == LSQ_HandleInvalid);
}

extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	//need to check container?
	if(it == NULL || it->container == LSQ_HandleInvalid || !LSQ_IsIteratorDereferencable) return NULL;
	return it->container->element;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	if(it == NULL) return NULL;
	LSQ_ShiftPosition(it, index);
	return it;
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it;
	ListItemPointerT h = (ListItemPointerT)handle;	
	if(h == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	while(h->prev != LSQ_HandleInvalid)
	{
		h = (ListItemPointerT)(h->prev);
	}
	it = createIterator(h->next);	
	return it;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it;
	ListItemPointerT h = (ListItemPointerT)handle;
	if(h == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	while(h->next != LSQ_HandleInvalid)
	{
		h = (ListItemPointerT)(h->next);
	}
	it = createIterator(h);	
	return it;
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	if(iterator == NULL) return;
	free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	//need to check container?
	if(it == NULL || it->container == LSQ_HandleInvalid || LSQ_IsIteratorPastRear(it)) return;
	it->container = (ListItemPointerT)(it->container->next);
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	//need to check container?
	if(it == NULL || it->container == LSQ_HandleInvalid || LSQ_IsIteratorBeforeFirst(it)) return;
	it->container = (ListItemPointerT)(it->container->prev);
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
	int i;
	ListIteratorPointerT it;
	it = (ListIteratorPointerT)iterator;	
	//need to check container?
	if(it == NULL || it->container == LSQ_HandleInvalid) return;
	i = 0;
	if(shift > 0)
	{
		for(i = 0; i < shift; i++)
		{
			if(it->container->next == LSQ_HandleInvalid)
			{				
				break;
			}
			it->container = (ListItemPointerT)(it->container->next);
		}
	}
	else
	{
		for(i = shift; i < 0; i++)
		{
			if(it->container->prev == LSQ_HandleInvalid)
			{				
				break;
			}
			it->container = (ListItemPointerT)(it->container->prev);
		}
	}
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == NULL) return;
	it = LSQ_GetFrontElement(it->container);
	if(it == NULL) return;
	LSQ_ShiftPosition(it, pos);
	((ListIteratorPointerT)iterator)->container = it->container;
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	if(it == NULL) return;
	LSQ_InsertElementBeforeGiven(it, element);
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = LSQ_GetPastRearElement(handle);
	if(it == NULL) return;
	LSQ_InsertElementBeforeGiven(it, element);	
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement)
{
	ListItemPointerT el;
	ListIteratorPointerT it;
	el = (ListItemPointerT)malloc(sizeof(ListItemT));	
	it = (ListIteratorPointerT)iterator;	
	if
	(
		it == NULL ||
		el == LSQ_HandleInvalid ||		
		LSQ_IsIteratorBeforeFirst(it)
	)
		return;	
	el->next = it->container;
	el->prev = (ListItemPointerT)(it->container->prev);
	((ListItemPointerT)(it->container->prev))->next = el;
	it->container->prev = el;
	el->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	* el->element = newElement;
	it->container = el;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	LSQ_DeleteGivenElement(it);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = LSQ_GetPastRearElement(handle);
	if(it == NULL) return;
	it->container = it->container->prev;
	LSQ_DeleteGivenElement(it);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator)
{
	ListItemPointerT a;
	ListItemPointerT c;
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == NULL || !LSQ_IsIteratorDereferencable(it)) return;
	a = (ListItemPointerT)(it->container->prev);
	c = (ListItemPointerT)(it->container->next);
	a->next = c;
	c->prev = a;
	free(it->container->element);
	free(it->container);
	it->container = c;
}