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
		struct ArrayItemTag * left;
		struct ArrayItemTag * right;
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

LSQ_BaseTypeT * LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	return !LSQ_IsIteratorDereferencable(it) ? LSQ_HandleInvalid : &(it->item->value);
	//DEREFERENCABLE == 0?
}

LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator)
{
	ArrayIteratorPointerT it = (ArrayIteratorPointerT)iterator;
	return !LSQ_IsIteratorDereferencable(it) ? 0 : it->item->key;	
	//DEREFERENCABLE == 0?
}

/* Функция, возвращающая итератор, ссылающийся на элемент с указанным ключом. Если элемент с данным ключом  *
 * отсутствует в контейнере, должен быть возвращен итератор PastRear.                                       */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
	ArrayItemPointerT i = NULL;
	AssocArrayPointerT h = (AssocArrayPointerT)handle;
	if(h == NULL) return LSQ_HandleInvalid;
	i = h->root;
	//i != NULL?
	while(i != NULL && i->key != index)
	{
		if(i->key > index)
		{
			i = i->left;
		}
		else
		{
			i = i->right;
		}
	}
	return i == NULL ? LSQ_GetPastRearElement(handle) : createIterator(h, i);
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
	AssocArrayPointerT h = (AssocArrayPointerT)handle;
	if(h == NULL) return LSQ_HandleInvalid;
	return createIterator(h, h->root);
}

/* Функция, возвращающая итератор, ссылающийся на фиктивный элемент, следующий за последним элементом контейнера */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
	AssocArrayPointerT h = (AssocArrayPointerT)handle;
	if(h == NULL) return LSQ_HandleInvalid;
	
	return NULL;
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
	free(iterator);
}

/* Следующие функции позволяют реализовать итерацию по элементам. При этом осуществляется проход только  *
 * по тем ключам, которые есть в контейнере.                                                             */
/* Функция, перемещающая итератор на один элемент вперед */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator);
/* Функция, перемещающая итератор на один элемент назад */
void LSQ_RewindOneElement(LSQ_IteratorT iterator);
/* Функция, перемещающая итератор на заданное смещение со знаком */
void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift);
/* Функция, устанавливающая итератор на элемент с указанным номером */
void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos);

/* Функция, добавляющая новую пару ключ-значение в контейнер. Если элемент с данным ключом существует,  *
 * его значение обновляется указанным.                                                                  */
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

/* Функция, удаляющая первый элемент контейнера */
void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
}

/* Функция, удаляющая последний элемент контейнера */
void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
}

/* Функция, удаляющая элемент контейнера, указываемый заданным ключом. */
void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key)
{
}