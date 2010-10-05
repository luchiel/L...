#include "linear_sequence.h"

#define IteratorInvalid NULL

typedef		
	struct ListItemTag
	{
		LSQ_BaseTypeT element;
		struct ListItemTag * next;
		struct ListItemTag * prev;
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
		ListPointerT container;
		ListItemPointerT item;
	}
		ListIteratorT, * ListIteratorPointerT;

static LSQ_IteratorT createIterator(ListPointerT handle, ListItemPointerT item)
{
	ListIteratorPointerT iterator = NULL;
	if(handle == LSQ_HandleInvalid || item == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	iterator = (ListIteratorPointerT)malloc(sizeof(ListIteratorT));
	if(iterator == IteratorInvalid) return IteratorInvalid;
	iterator->container = (ListPointerT)handle;
	iterator->item = item;
	return iterator;
}

LSQ_HandleT LSQ_CreateSequence(void)
{
	ListPointerT list = NULL;
	list = (ListPointerT)malloc(sizeof(ListT));
	if(list == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	list->bfirst = (ListItemPointerT)malloc(sizeof(ListItemT));
	if(list->bfirst == LSQ_HandleInvalid)
	{
		free(list);
		return LSQ_HandleInvalid;
	}
	list->prear = (ListItemPointerT)malloc(sizeof(ListItemT));	
	if(list->prear == LSQ_HandleInvalid)
	{
		free(list->bfirst);
		free(list);
		return LSQ_HandleInvalid;
	}	
	list->prear->prev = list->bfirst;
	list->prear->next = NULL;
	list->bfirst->prev = NULL;
	list->bfirst->next = list->prear;	
	list->count = 0;
	return list; 
}

void LSQ_DestroySequence(LSQ_HandleT handle)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return;
	while(! LSQ_IsIteratorPastRear(it))
	{
		free(it->item->prev);
		LSQ_AdvanceOneElement(it);
	}
	free(it->item->prev);
	free(it->item);
	free(it->container);
	LSQ_DestroyIterator(it);
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
	if(handle == LSQ_HandleInvalid) return -1;
	return ((ListPointerT)handle)->count;
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->item != it->container->bfirst && it->item != it->container->prear);
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->item == it->container->prear);
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return -1;
	return (it->item == it->container->bfirst);
}

LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->item == LSQ_HandleInvalid || !LSQ_IsIteratorDereferencable(it))
		return LSQ_HandleInvalid;
	return &(it->item->element);
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return IteratorInvalid;
	LSQ_ShiftPosition(it, index);
	return it;
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = NULL;
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	it = (ListIteratorPointerT)createIterator((ListPointerT)handle, ((ListPointerT)handle)->bfirst);
	LSQ_AdvanceOneElement(it);
	return it;
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
	ListIteratorPointerT it = NULL;	
	if(handle == LSQ_HandleInvalid) return LSQ_HandleInvalid;
	it = (ListIteratorPointerT)createIterator((ListPointerT)handle, ((ListPointerT)handle)->prear);
	return it;
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	if(iterator == IteratorInvalid) return;
	free(iterator);
}

void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->item == LSQ_HandleInvalid || LSQ_IsIteratorPastRear(it)) return;
	it->item = it->item->next;
}

void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || it->item == LSQ_HandleInvalid || LSQ_IsIteratorBeforeFirst(it)) return;
	it->item = it->item->prev;
}

void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
	int i = 0;
	ListIteratorPointerT it;
	it = (ListIteratorPointerT)iterator;		
	if(it == IteratorInvalid || it->item == LSQ_HandleInvalid) return;	
	if(shift > 0)
	{
		for(i = 0; i < shift; i++)
		{
			if(LSQ_IsIteratorPastRear(it))
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
			if(LSQ_IsIteratorBeforeFirst(it))
			{				
				break;
			}
			LSQ_RewindOneElement(it);
		}
	}
}

void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid) return;
	it = (ListIteratorPointerT)LSQ_GetFrontElement(it->container);	
	LSQ_ShiftPosition(it, pos);
	((ListIteratorPointerT)iterator)->item = it->item;
	LSQ_DestroyIterator(it);
}

void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetFrontElement(handle);
	if(it == IteratorInvalid) return;
	LSQ_InsertElementBeforeGiven(it, element);
	LSQ_DestroyIterator(it);
}

void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element)
{
	ListIteratorPointerT it = (ListIteratorPointerT)LSQ_GetPastRearElement(handle);
	if(it == IteratorInvalid) return;
	LSQ_InsertElementBeforeGiven(it, element);	
	LSQ_DestroyIterator(it);
}

void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement)
{
	ListItemPointerT el = NULL;
	ListIteratorPointerT it = NULL;	
	it = (ListIteratorPointerT)iterator;	
	if(it == IteratorInvalid || LSQ_IsIteratorBeforeFirst(it)) return;
	el = (ListItemPointerT)malloc(sizeof(ListItemT));	
	if(el == LSQ_HandleInvalid) return;
	el->next = it->item;
	el->prev = it->item->prev;
	it->item->prev->next = el;
	it->item->prev = el;	
	el->element = newElement;
	it->item = el;
	it->container->count++;
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
	it->item = it->item->prev;
	LSQ_DeleteGivenElement(it);
	LSQ_DestroyIterator(it);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator)
{
	ListItemPointerT a = NULL;
	ListItemPointerT c = NULL;
	ListIteratorPointerT it = (ListIteratorPointerT)iterator;
	if(it == IteratorInvalid || !LSQ_IsIteratorDereferencable(it)) return;
	a = it->item->prev;
	c = it->item->next;
	a->next = c;
	c->prev = a;
	free(it->item);
	it->item = c;
	it->container->count--;
}