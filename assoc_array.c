//#define NDEBUG
#include <assert.h>
#include "linear_sequence_assoc.h"

typedef
	enum
	{
		IT_DEREFERENCABLE,
		IT_BEFOREFIRST,
		IT_PASTREAR,
	}
		IteratorTypeT;

typedef
	struct ArrayItemTag
	{
		ArrayItemTag left;
		ArrayItemTag right;
		LSQ_IntegerIndexT key;
		LSQ_BaseTypeT value;
		int height;
	}
		ArrayItemT, * ArrayItemPointerT;

typedef
	struct
	{
		ArrayItemPointerT root;
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
	if(iterator == NULL) return LSQ_HandleInvalid;	
	it->container = (AssocArrayPointerT)handle;
	it->item = item;
	return it;	
}

LSQ_HandleT LSQ_CreateSequence(void)
{
	AssocArrayPointerT h = NULL;	
	h = (AssocArrayPointerT)malloc(sizeof(AssocArrayT));
	if(h == NULL) return LSQ_HandleInvalid;
	h->root = NULL;
	h->count = 0;
	return h;
}

void LSQ_DestroySequence(LSQ_HandleT handle)
{
	//
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
	return handle == NULL ? -1 : ((AssocArrayPointerT)handle)->count;
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	if(it == NULL) return 0;
	//assert(it->container != NULL);
	return it->item == NULL ? 0 : IT_DEREFERENCABLE;
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
	//?
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
	//?
}

LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	return !LSQ_IsIteratorDereferencable(it) ? LSQ_HandleInvalid : * (it->item->value);
	//DEREFERENCABLE == 0?
}

LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	return !LSQ_IsIteratorDereferencable(it) ? LSQ_HandleInvalid : it->item->key;	
	//DEREFERENCABLE == 0?
}

/* �������, ������������ ��������, ����������� �� ������� � ��������� ������. ���� ������� � ������ ������  *
 * ����������� � ����������, ������ ���� ��������� �������� PastRear.                                       */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index);

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	AssocArrayPointerT h = (AssocArrayPointerT)handle;
	if(h == NULL) return LSQ_HandleInvalid;
	return createIterator(h, h->root);
}

/* �������, ������������ ��������, ����������� �� ��������� �������, ��������� �� ��������� ��������� ���������� */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle);

/* �������, ������������ �������� � �������� ������������ � ������������� ������������� ��� ������ */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator);

/* ��������� ������� ��������� ����������� �������� �� ���������. ��� ���� �������������� ������ ������  *
 * �� ��� ������, ������� ���� � ����������.                                                             */
/* �������, ������������ �������� �� ���� ������� ������ */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator);
/* �������, ������������ �������� �� ���� ������� ����� */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator);
/* �������, ������������ �������� �� �������� �������� �� ������ */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift);
/* �������, ��������������� �������� �� ������� � ��������� ������� */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos);

/* �������, ����������� ����� ���� ����-�������� � ���������. ���� ������� � ������ ������ ����������,  *
 * ��� �������� ����������� ���������.                                                                  */
extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value);

/* �������, ��������� ������ ������� ���������� */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle);
/* �������, ��������� ��������� ������� ���������� */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle);
/* �������, ��������� ������� ����������, ����������� �������� ������. */
extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key);