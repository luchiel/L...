#include "linear_sequence.h"

#define IteratorInvalid NULL

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
		ListItemPointerT bfirst;
		ListItemPointerT prear;
		LSQ_IntegerIndexT count;
	}
		ListT, * ListPointerT;
		
typedef
	struct
	{
		ListItemPointerT container;
	}
		ListIteratorT, * ListIteratorPointerT;

static LSQ_IteratorT createIterator(LSQ_HandleT handle);

static LSQ_IteratorT createIterator(LSQ_HandleT handle)
{
	ListIteratorPointerT iterator = NULL;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	iterator = (ListIteratorPointerT)malloc(sizeof(ListIteratorT));
	if(iterator == IteratorInvalid) return IteratorInvalid;
	iterator->container = (ListItemPointerT)handle;
	return iterator;
}

extern LSQ_HandleT LSQ_CreateSequence(void)
{
	ListItemPointerT bfirst = NULL;
	ListItemPointerT prear = NULL;
	bfirst = (ListItemPointerT)malloc(sizeof(ListItemT));
	if(bfirst == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	prear = (ListItemPointerT)malloc(sizeof(ListItemT));	
	if(prear == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	prear->prev = bfirst;
	prear->next = NULL;
	//prear->element = NULL;
	prear->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	if(prear->element == NULL) return NULL;
	bfirst->prev = NULL;
	bfirst->next = prear;
	//bfirst->element = NULL;
	bfirst->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	if(bfirst->element == NULL) return NULL;
	return bfirst; 
}

extern void LSQ_DestroySequence(LSQ_HandleT handle)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return;
	while(! LSQ_IsIteratorPastRear(it))
	{
		free(((ListItemPointerT)(it->container->prev))->element);
		free(it->container->prev);
		LSQ_AdvanceOneElement(it);
	}
	free(((ListItemPointerT)(it->container->prev))->element);
	free(it->container->prev);
	free(((ListItemPointerT)(it->container))->element);
	free(it->container);
	LSQ_DestroyIterator(it);
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
	int i = 0;
	ListIteratorPointerT it = LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return -1;	
	while(! LSQ_IsIteratorPastRear(it))		
	{
		LSQ_AdvanceOneElement(it);
		i++;
	}
	LSQ_DestroyIterator(it);
	return i;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->container->prev != LSQ_HandleInvalid && it->container->next != LSQ_HandleInvalid);
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->container->next == LSQ_HandleInvalid);
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->container->prev == LSQ_HandleInvalid);
}

extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->container == LSQ_HandleInvalid || !LSQ_IsIteratorDereferencable(it))
		return LSQ_HandleInvalid;
	return it->container->element;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return IteratorInvalid;
	LSQ_ShiftPosition(it, index);
	return it;
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = NULL;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	it = (ListIteratorPointerT)createIterator(handle);	
	while(! LSQ_IsIteratorBeforeFirst(it))		
	{
		LSQ_RewindOneElement(it);
	}
	LSQ_AdvanceOneElement(it);
	return it;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = NULL;	
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	it = (ListIteratorPointerT)createIterator(handle);
	while(!LSQ_IsIteratorPastRear(it))	
	{
		LSQ_AdvanceOneElement(it);
	}	
	return it;
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	if(iterator == IteratorInvalid) return;
	free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->container == LSQ_HandleInvalid || LSQ_IsIteratorPastRear(it)) return;
	it->container = (ListItemPointerT)(it->container->next);
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->container == LSQ_HandleInvalid || LSQ_IsIteratorBeforeFirst(it)) return;
	it->container = (ListItemPointerT)(it->container->prev);
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
	int i = 0;
	ListIteratorPointerT it;
	it = (ListIteratorPointerT)iterator;		
	if(it == IteratorInvalid || it->container == LSQ_HandleInvalid) return;	
	if(shift > 0)
	{
		for(i = 0; i < shift; i++)
		{
			if(it->container->next == LSQ_HandleInvalid)
			{				
				break;
			}
			LSQ_AdvanceOneElement(it);
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
			LSQ_RewindOneElement(it);
		}
	}
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return;
	it = (ListIteratorPointerT)LSQ_GetFrontElement(it->container);
	if(it == IteratorInvalid) return;
	LSQ_ShiftPosition(it, pos);
	((ListIteratorPointerT)iterator)->container = it->container;
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return;
	LSQ_InsertElementBeforeGiven(it, element);
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetPastRearElement(handle);
	if(it == IteratorInvalid) return;
	LSQ_InsertElementBeforeGiven(it, element);	
	LSQ_DestroyIterator(it);
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement)
{
	ListItemPointerT el = NULL;
	ListIteratorPointerT it = NULL;	
	it = (ListIteratorPointerT)iterator;	
	if
	(
		it == IteratorInvalid ||
		LSQ_IsIteratorBeforeFirst(it)
	)
		return;
	el = (ListItemPointerT)malloc(sizeof(ListItemT));	
	if(el == LSQ_HandleInvalid) return;
	el->next = it->container;
	el->prev = (ListItemPointerT)(it->container->prev);
	((ListItemPointerT)(it->container->prev))->next = el;
	it->container->prev = el;	
	el->element = (LSQ_BaseTypeT *)malloc(sizeof(LSQ_BaseTypeT));
	if(el->element == NULL) return;
	* el->element = newElement;
	it->container = el;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	LSQ_DeleteGivenElement(it);
	LSQ_DestroyIterator(it);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetPastRearElement(handle);
	if(it == IteratorInvalid) return;
	it->container = it->container->prev;
	LSQ_DeleteGivenElement(it);
	LSQ_DestroyIterator(it);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator)
{
	ListItemPointerT a = NULL;
	ListItemPointerT c = NULL;
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid || !LSQ_IsIteratorDereferencable(it)) return;
	a = (ListItemPointerT)(it->container->prev);
	c = (ListItemPointerT)(it->container->next);
	a->next = c;
	c->prev = a;
	free(it->container->element);
	free(it->container);
	it->container = c;
}