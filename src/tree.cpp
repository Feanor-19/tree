#include "tree.h"

#include <stdio.h>
#include <assert.h>
#include <memory.h>

TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
                       int (*data_dtor_func_ptr)(void *data_ptr)
#ifdef TREE_DO_DUMP
                       , int (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       TreeOrigInfo orig_info
#endif
                    )
{
    assert(tree_ptr);
    assert(data_size_in_bytes > 0);

    tree_ptr->data_size             = data_size_in_bytes;
    tree_ptr->data_dtor_func_ptr    = data_dtor_func_ptr;

#ifdef TREE_DO_DUMP
    tree_ptr->print_data_func_ptr   = print_data_func_ptr;
    tree_ptr->orig_info             = orig_info;
#endif

    return TREE_STATUS_OK;
}

TreeStatus tree_dtor( Tree *tree_ptr )
{
    assert(tree_ptr);

    TreeNode *curr_node_ptr = tree_ptr->head_of_all_nodes;
    while ( curr_node_ptr != NULL )
    {
        TreeNode *tmp = curr_node_ptr->prev;
        op_del_TreeNode(tree_ptr, curr_node_ptr);
        curr_node_ptr = tmp;
    }

    return TREE_STATUS_OK;
}

TreeStatus tree_insert_root( Tree *tree_ptr, void *data )
{
    TREE_SELFCHECK(tree_ptr);
    assert(data);

    if (tree_ptr->root)
        return TREE_STATUS_WARNING_ROOT_ALREADY_EXISTS;

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data);
    if (!new_node)
        return TREE_STATUS_ERROR_MEM_ALLOC;

    tree_ptr->root = new_node;

    return TREE_STATUS_OK;
}

TreeStatus tree_insert_data_as_left_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);
    assert(data);

    if ( node_ptr->left )
        return TREE_STATUS_WARNING_LEFT_CHILD_IS_OCCUPIED;

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data);
    if (!new_node)
        return TREE_STATUS_ERROR_MEM_ALLOC;

    node_ptr->left = new_node;

    return TREE_STATUS_OK;
}

TreeStatus tree_insert_data_as_right_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);
    assert(data);

    if ( node_ptr->right )
        return TREE_STATUS_WARNING_RIGHT_CHILD_IS_OCCUPIED;

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data);
    if (!new_node)
        return TREE_STATUS_ERROR_MEM_ALLOC;

    node_ptr->right = new_node;

    return TREE_STATUS_OK;
}

TreeStatus tree_get_data( Tree *tree_ptr, TreeNode *node_ptr, void *ret )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);
    assert(ret);

    memcpy(ret, node_ptr->data_ptr, tree_ptr->data_size);

    return TREE_STATUS_OK;
}

TreeNode *tree_get_root( Tree *tree_ptr )
{
    assert(tree_ptr);

    return tree_ptr->root;
}

TreeNode* tree_get_left_child( Tree *tree_ptr, TreeNode *node_ptr )
{
    assert(node_ptr);

    return node_ptr->left;
}

TreeNode* tree_get_right_child( Tree *tree_ptr, TreeNode *node_ptr )
{
    assert(node_ptr);

    return node_ptr->right;
}

TreeStatus tree_delete_root( Tree *tree_ptr )
{
    TREE_SELFCHECK(tree_ptr);

    if (!tree_ptr->root)
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NULL_NODE;

    op_del_TreeNode(tree_ptr, tree_ptr->root);

    tree_ptr->root = NULL;

    return TREE_STATUS_OK;
}

TreeStatus tree_delete_left_child( Tree *tree_ptr, TreeNode *node_ptr )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);

    if ( !node_ptr->left )
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NULL_NODE;

    if ( !is_node_leaf(tree_ptr, node_ptr->left) )
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NOT_A_LEAF;

    op_del_TreeNode(tree_ptr, node_ptr->left);

    node_ptr->left = NULL;

    return TREE_STATUS_OK;
}

TreeStatus tree_delete_right_child( Tree *tree_ptr, TreeNode *node_ptr )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);

    if ( !node_ptr->right )
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NULL_NODE;

    if ( !is_node_leaf(tree_ptr, node_ptr->right) )
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NOT_A_LEAF;

    op_del_TreeNode(tree_ptr, node_ptr->right);

    node_ptr->right = NULL;

    return TREE_STATUS_OK;
}

int is_node_leaf( Tree *tree_ptr, TreeNode* node_ptr)
{
    assert(node_ptr);

    return ( !node_ptr->left && !node_ptr->right );
}

static TreeNode *op_new_TreeNode( Tree *tree_ptr, void *data )
{
    assert(data);

    char *new_mem = (char*) calloc( 1, sizeof(TreeNode) + tree_ptr->data_size );
    if (!new_mem)
        return NULL;

    memcpy( new_mem + sizeof(TreeNode), data, tree_ptr->data_size );

    TreeNode *new_node = (TreeNode *) new_mem;
    new_node->data_ptr = (void*) (new_mem + sizeof(TreeNode));

    TreeNode *tmp = tree_ptr->head_of_all_nodes;
    tree_ptr->head_of_all_nodes = new_node;
    new_node->prev = tmp;
    if (tmp)
    {
        tmp->next = new_node;
    }

    return new_node;
}

static void op_del_TreeNode( Tree *tree_ptr, TreeNode *node_ptr )
{
    assert(tree_ptr);
    assert(node_ptr);

    tree_ptr->data_dtor_func_ptr( node_ptr->data_ptr );

    node_ptr->left = NULL;
    node_ptr->right = NULL;
    node_ptr->data_ptr = NULL;

    TreeNode *next = node_ptr->next;
    TreeNode *prev = node_ptr->prev;
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;

    if (!next)
    {
       tree_ptr->head_of_all_nodes = prev;
    }

    node_ptr->next = NULL;
    node_ptr->prev = NULL;

    free(node_ptr);
}

#ifdef TREE_DO_DUMP
static tree_verify_t tree_verify( Tree *tree_ptr )
{
    // TODO -

    return 0;
}

void tree_dump_( Tree *tree_ptr,
                 tree_verify_t verify_res,
                 const char *file,
                 const int line,
                 const char *func)
{

}


#endif /* TREE_DO_DUMP */
