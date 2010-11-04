//#define NDEBUG
#include <assert.h>
#include "linear_sequence_assoc.h"
//#include <stdio.h>

typedef
	enum
    {
		IT_DEREFERENCABLE,
		IT_BEFOREFIRST,
		IT_PASTREAR
	}
        IteratorTypeT;

typedef
    enum
    {
        RM_LEFT,
        RM_RIGHT,
    }
        RotateModeT;

typedef
    struct TreeNodeTag
    {
        struct TreeNodeTag * left;
        struct TreeNodeTag * right;
        struct TreeNodeTag * parent;
        LSQ_IntegerIndexT key;
        LSQ_BaseTypeT value;
        int height;
    }
        TreeNodeT, * TreeNodePointerT;

typedef
    struct
    {
        TreeNodePointerT root;
        LSQ_IntegerIndexT count;
    }
        TreeT, * TreePointerT;

typedef
    struct
    {
        TreePointerT container;
        TreeNodePointerT item;
        IteratorTypeT type;
    }
        TreeIteratorT, * TreeIteratorPointerT;


static TreeNodePointerT createTreeNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, TreeNodePointerT parent);
static LSQ_IteratorT createIterator(LSQ_HandleT handle, IteratorTypeT type, TreeNodePointerT item);
static void freeTreeNode(TreeNodePointerT node);

static void checkParentAndReplaceChild(TreeIteratorPointerT it, TreeNodePointerT newChild);

static void rotateTreeNodeLeft(TreeNodePointerT oldRoot, TreeNodePointerT newRoot);
static void rotateTreeNodeRight(TreeNodePointerT oldRoot, TreeNodePointerT newRoot);
static void rotateTreeNode(LSQ_IteratorT iterator, RotateModeT mode);

static void updateTreeNodeHeight(TreeNodePointerT node);
static int getTreeNodeHeight(TreeNodePointerT node);
static int getTreeNodeBalanceParameter(TreeNodePointerT node);
static void fixTreeNodeBalance(TreeIteratorPointerT it, int stopCriterion);

TreeNodePointerT createTreeNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, TreeNodePointerT parent)
{
    TreeNodePointerT node = (TreeNodePointerT)malloc(sizeof(TreeNodeT));
    if(node == NULL)
        return LSQ_HandleInvalid;
    node->parent = parent;
    node->height = 1;
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

LSQ_IteratorT createIterator(LSQ_HandleT handle, IteratorTypeT type, TreeNodePointerT item)
{
    TreeIteratorPointerT it = NULL;
    if(handle == NULL)
        return LSQ_HandleInvalid;
    assert(item != NULL || type != IT_DEREFERENCABLE);

    it = (TreeIteratorPointerT)malloc(sizeof(TreeIteratorT));
    if(it == NULL)
        return LSQ_HandleInvalid;	

    it->container = (TreePointerT)handle;
    it->type = type;
    it->item = item;

    return it;	
}

void freeTreeNode(TreeNodePointerT node)
{
    if(node == NULL)
        return;
    freeTreeNode(node->left);
    freeTreeNode(node->right);
    free(node);
}

void checkParentAndReplaceChild(TreeIteratorPointerT it, TreeNodePointerT newChild)
{
    if(it->container->root == it->item)
        it->container->root = newChild;
    else if(it->item->parent->left == it->item)
        it->item->parent->left = newChild;
    else
        it->item->parent->right = newChild;
}

void rotateTreeNodeLeft(TreeNodePointerT oldRoot, TreeNodePointerT newRoot)
{
    oldRoot->right = newRoot->left;
    if(oldRoot->right != NULL)
        oldRoot->right->parent = oldRoot;
    newRoot->left = oldRoot;
    newRoot->parent = oldRoot->parent;    
}

void rotateTreeNodeRight(TreeNodePointerT oldRoot, TreeNodePointerT newRoot)
{
    oldRoot->left = newRoot->right;
    if(oldRoot->left != NULL)
        oldRoot->left->parent = oldRoot;
    newRoot->right = oldRoot;
    newRoot->parent = oldRoot->parent;
}

void rotateTreeNode(LSQ_IteratorT iterator, RotateModeT mode)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    TreeNodePointerT oldRoot = NULL;
    TreeNodePointerT newRoot = NULL;

    if(it == NULL || !LSQ_IsIteratorDereferencable(it))
        return;
    assert(it->item != NULL);
    assert(mode == RM_LEFT || mode == RM_RIGHT);

    oldRoot = it->item;
    newRoot = mode == RM_LEFT ? it->item->right : it->item->left;

    checkParentAndReplaceChild(it, newRoot);
    
    mode == RM_LEFT ? rotateTreeNodeLeft(oldRoot, newRoot) : rotateTreeNodeRight(oldRoot, newRoot);
    oldRoot->parent = newRoot;

    updateTreeNodeHeight(oldRoot);
    updateTreeNodeHeight(newRoot);
}

void updateTreeNodeHeight(TreeNodePointerT node)
{
    int left;
    int right;
    if(node == NULL)
        return;
    left = getTreeNodeHeight(node->left);
    right = getTreeNodeHeight(node->right);
    node->height = 1 + (left > right ? left : right);
}

int getTreeNodeHeight(TreeNodePointerT node)
{
    return node == NULL ? 0 : node->height;
}

int getTreeNodeBalanceParameter(TreeNodePointerT node)
{
    return getTreeNodeHeight(node->left) - getTreeNodeHeight(node->right);
}

/*static void OUT(TreeNodePointerT n)
{
    if(n == NULL) return;
    printf("%d ", n->key);
    if(n->left != NULL) printf("%d ", n->left->key);
    if(n->right != NULL) printf("%d ", n->right->key);
    printf("\n");
    OUT(n->left);
    OUT(n->right);
    return;
}*/

void fixTreeNodeBalance(TreeIteratorPointerT it, int stopCriterion)
{
    TreeIteratorPointerT tmp = (TreeIteratorPointerT)createIterator(it->container, IT_DEREFERENCABLE, it->item);
    assert(LSQ_IsIteratorDereferencable(it));
    do
    {
        updateTreeNodeHeight(it->item);
        if(getTreeNodeBalanceParameter(it->item) == 2)
        {
            tmp->item = it->item->left;
            if(getTreeNodeBalanceParameter(it->item->left) < 0)
                rotateTreeNode(tmp, RM_LEFT);
            rotateTreeNode(it, RM_RIGHT);
        }
        else if(getTreeNodeBalanceParameter(it->item) == -2)
        {
            tmp->item = it->item->right;
            if(getTreeNodeBalanceParameter(it->item->right) > 0)
                rotateTreeNode(tmp, RM_RIGHT);
            rotateTreeNode(it, RM_LEFT);
        }
        it->item = it->item->parent;
        //printf("\n");
        //OUT(it->container->root);
    }    
    while(it->item != NULL && abs(getTreeNodeBalanceParameter(it->item)) != stopCriterion);
    LSQ_DestroyIterator(tmp);
}

LSQ_HandleT LSQ_CreateSequence(void)
{
    TreePointerT h = NULL;	

    h = (TreePointerT)malloc(sizeof(TreeT));
    if(h == NULL)
        return LSQ_HandleInvalid;	

    h->root = NULL;
    h->count = 0;	

    return h;
}

void LSQ_DestroySequence(LSQ_HandleT handle)
{
    TreePointerT h = (TreePointerT)handle;
    if(h != NULL)
        freeTreeNode(h->root);
    free(h);
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle)
{
    return handle == NULL ? -1 : ((TreePointerT)handle)->count;
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator)
{	
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;    
    return (it == NULL) ? 0 : it->type == IT_DEREFERENCABLE;
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;    
    return (it == NULL) ? 0 : it->type == IT_PASTREAR;
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    return (it == NULL) ? 0 : it->type == IT_BEFOREFIRST;
}

LSQ_BaseTypeT * LSQ_DereferenceIterator(LSQ_IteratorT iterator)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    return !LSQ_IsIteratorDereferencable(it) ? LSQ_HandleInvalid : &(it->item->value);	
}

LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    return !LSQ_IsIteratorDereferencable(it) ? 0 : it->item->key;	
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index)
{
    TreeIteratorPointerT it = NULL;    
    if(handle == NULL)
        return LSQ_HandleInvalid;
    it = (TreeIteratorPointerT)createIterator(handle, IT_DEREFERENCABLE, ((TreePointerT)handle)->root);
    if(it == NULL)
        return LSQ_HandleInvalid;
    while(
        it->item != NULL && (
            (it->item->key > index && it->item->left != NULL) ||
            (it->item->key < index && it->item->right != NULL)
        )
    )
    {
        if(it->item->key > index)
            it->item = it->item->left;
        else if(it->item->key < index)
            it->item = it->item->right;
    }
    if(it->item == NULL || it->item->key != index)
        it->type = IT_PASTREAR;
    return it;
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle)
{
    TreePointerT h = (TreePointerT)handle;
    TreeIteratorPointerT it = (TreeIteratorPointerT)createIterator(handle, IT_BEFOREFIRST, NULL);
    LSQ_AdvanceOneElement(it);
    return it;
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle)
{
    TreePointerT h = (TreePointerT)handle;
    return createIterator(handle, IT_PASTREAR, NULL);
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator)
{
    free(iterator);
}

void LSQ_AdvanceOneElement(LSQ_IteratorT iterator)
{
    int leftChild = 0;
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    if(it == NULL || LSQ_IsIteratorPastRear(it))
        return;
    
    if(LSQ_IsIteratorBeforeFirst(it) && it->container->root == NULL)
    {
        it->type = IT_PASTREAR;
        return;
    }

    assert(LSQ_IsIteratorBeforeFirst(it) || it->item != NULL);

    if(LSQ_IsIteratorDereferencable(it) && it->item->right == NULL)
    {
        while(it->item->parent != NULL && !leftChild)
        {
            leftChild = it->item == it->item->parent->left;
            it->item = it->item->parent;
        }
        if(!leftChild)
        {
            it->item = NULL;
            it->type = IT_PASTREAR;
        }
    }
    else
    {
        if(LSQ_IsIteratorBeforeFirst(it))
        {
            it->item = it->container->root;
            it->type = IT_DEREFERENCABLE;
        }
        else
            it->item = it->item->right;
        while(it->item->left != NULL)
            it->item = it->item->left;
    }
}

void LSQ_RewindOneElement(LSQ_IteratorT iterator)
{
    int rightChild = 0;
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    if(it == NULL || LSQ_IsIteratorBeforeFirst(it))
        return;
    
    if(LSQ_IsIteratorPastRear(it) && it->container->root == NULL)
    {
        it->type = IT_BEFOREFIRST;
        return;
    }

    assert(LSQ_IsIteratorPastRear(it) || it->item != NULL);

    if(LSQ_IsIteratorDereferencable(it) && it->item->left == NULL)
    {
        while(it->item->parent != NULL && !rightChild)
        {
            rightChild = it->item == it->item->parent->right;
            it->item = it->item->parent;
        }
        if(!rightChild)
        {
            it->item = NULL;
            it->type = IT_BEFOREFIRST;
        }
    }
    else
    {
        if(LSQ_IsIteratorPastRear(it))
        {
            it->item = it->container->root;
            it->type = IT_DEREFERENCABLE;
        }
        else
            it->item = it->item->left;
        while(it->item->right != NULL)
            it->item = it->item->right;
    }
}

void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    if(it == NULL)
        return;
    while(shift-- > 0)
        LSQ_AdvanceOneElement(it);
    shift++;
    while(shift++ < 0)
        LSQ_RewindOneElement(it);
}

void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)iterator;
    if(it == NULL)
        return;
    it->type = IT_BEFOREFIRST;
    LSQ_ShiftPosition(it, pos + 1);    
}

void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value)
{
    TreePointerT h = (TreePointerT)handle;
    TreeIteratorPointerT it = NULL;
    TreeNodePointerT node = NULL;
    if(h == NULL)
        return;
    if(h->root == NULL)
    {
        h->root = createTreeNode(key, value, NULL);
        h->count++;
        return;
    }
    it = (TreeIteratorPointerT)LSQ_GetElementByIndex(handle, key);
    if(it == NULL)
        return;
    if(LSQ_IsIteratorDereferencable(it))
    {
        it->item->value = value;
        LSQ_DestroyIterator(it);
        return;
    }
    node = createTreeNode(key, value, it->item);
    if(it->item->key > key)
        it->item->left = node;
    else
        it->item->right = node;
    it->type = IT_DEREFERENCABLE;
    h->count++;
    
    fixTreeNodeBalance(it, 0);
    
    LSQ_DestroyIterator(it);
}

void LSQ_DeleteFrontElement(LSQ_HandleT handle)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)LSQ_GetFrontElement(handle);
    if(it == NULL)
        return;
    LSQ_DeleteElement(handle, LSQ_GetIteratorKey(it));
    LSQ_DestroyIterator(it);
}

void LSQ_DeleteRearElement(LSQ_HandleT handle)
{
    TreeIteratorPointerT it = (TreeIteratorPointerT)LSQ_GetPastRearElement(handle);
    if(it == NULL)
        return;
    LSQ_RewindOneElement(it);
    LSQ_DeleteElement(handle, LSQ_GetIteratorKey(it));
    LSQ_DestroyIterator(it);
}

void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key)
{
    int tmp = 0;
    TreeNodePointerT node = NULL;
    TreeIteratorPointerT parent = NULL;
    TreeIteratorPointerT it = (TreeIteratorPointerT)LSQ_GetElementByIndex(handle, key);

    if(it == NULL || !LSQ_IsIteratorDereferencable(it))
        return;
    assert(it->item != NULL);

    if(it->item->parent != NULL)
        parent = (TreeIteratorPointerT)createIterator(handle, IT_DEREFERENCABLE, it->item->parent);
    if(it->item->left == NULL && it->item->right == NULL)
    {   
        checkParentAndReplaceChild(it, NULL);
        freeTreeNode(it->item);        
    }
    else if(it->item->left != NULL && it->item->right != NULL)
    {   
        node = it->item->left;
        while(node->right != NULL)
            node = node->right;
        it->item->value = node->value;        
        it->item->key = node->key;
        if(node->parent->left == node)
            node->parent->left = NULL;
        else
            node->parent->right = NULL;
        parent = (TreeIteratorPointerT)createIterator(handle, IT_DEREFERENCABLE, node->parent);
        freeTreeNode(node);
    }
    else
    {
        node = it->item->left == NULL ? it->item->right : it->item->left;
        checkParentAndReplaceChild(it, node);
        node->parent = it->item->parent;
        it->item->left = NULL;
        it->item->right = NULL;
        freeTreeNode(it->item);
    }
    it->container->count--;

    if(parent != NULL)        
        fixTreeNodeBalance(parent, 1);        
    
    LSQ_DestroyIterator(it);
    LSQ_DestroyIterator(parent);
}