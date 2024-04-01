#include "tree.h"
// #include "tree_dump.h"
#include "tree_alloc.h"

#include <stdio.h>
#include <assert.h>
#include <memory.h>


TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
#ifdef TREE_DO_DUMP
                       void (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       TreeOrigInfo orig_info,
#endif
                       size_t typical_num_of_nodes,
                       void (*data_dtor_func_ptr)(void *data_ptr)
                    )
{
    assert(tree_ptr);
    assert(data_size_in_bytes > 0);

    tree_ptr->data_size             = data_size_in_bytes;
    tree_ptr->data_dtor_func_ptr    = data_dtor_func_ptr;
    tree_ptr->nodes_count           = 0;
    tree_ptr->depth                 = 0;
    tree_ptr->root                  = NULL;

    tree_ptr->typical_num_of_nodes  = typical_num_of_nodes;

    _tree_alloc_init( sizeof(TreeNode) + data_size_in_bytes, typical_num_of_nodes );

#ifdef TREE_DO_DUMP
    tree_ptr->print_data_func_ptr   = print_data_func_ptr;
    tree_ptr->orig_info             = orig_info;
#endif

    return TREE_STATUS_OK;
}

TreeStatus tree_dtor( Tree *tree_ptr )
{
    assert(tree_ptr);

    // TODO - осознать, нужно ли это
    /*
    TreeNode *curr_node_ptr = tree_ptr->head_of_all_nodes;
    while ( curr_node_ptr != NULL )
    {
        TreeNode *tmp = curr_node_ptr->next;
        op_del_TreeNode(tree_ptr, curr_node_ptr);
        curr_node_ptr = tmp;
    }
    */

    _tree_alloc_deinit();

    tree_ptr->root                  = NULL;
    tree_ptr->nodes_count           = 0;
    tree_ptr->depth                 = 0;
    tree_ptr->data_size             = 0;
    tree_ptr->data_dtor_func_ptr    = NULL;

#ifdef TREE_DO_DUMP
    tree_ptr->print_data_func_ptr   = NULL;
    tree_ptr->orig_info             = {};
#endif

    return TREE_STATUS_OK;
}

TreeStatus tree_insert_root( Tree *tree_ptr, void *data )
{
    TREE_SELFCHECK(tree_ptr);
    assert(data);

    if (tree_ptr->root)
        return TREE_STATUS_WARNING_ROOT_ALREADY_EXISTS;

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data, NULL);
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

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data, node_ptr);
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

    TreeNode *new_node = op_new_TreeNode(tree_ptr, data, node_ptr);
    if (!new_node)
        return TREE_STATUS_ERROR_MEM_ALLOC;

    node_ptr->right = new_node;

    return TREE_STATUS_OK;
}

TreeStatus tree_get_data( const Tree *tree_ptr, const TreeNode *node_ptr, void *ret )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);
    assert(ret);

    memcpy(ret, node_ptr->data_ptr, tree_ptr->data_size);

    return TREE_STATUS_OK;
}

void *tree_get_data_ptr( const TreeNode *node_ptr )
{
    assert(node_ptr);

    return node_ptr->data_ptr;
}

TreeNode *tree_get_parent( const TreeNode *node_ptr )
{
    assert(node_ptr);
    return node_ptr->parent;
}

TreeStatus tree_change_data( Tree *tree_ptr, TreeNode *node_ptr, void *new_data )
{
    TREE_SELFCHECK(tree_ptr);
    assert(node_ptr);
    assert(new_data);

    if (tree_ptr->data_dtor_func_ptr) tree_ptr->data_dtor_func_ptr(node_ptr->data_ptr);
    memcpy( node_ptr->data_ptr, new_data, tree_ptr->data_size );

    return TREE_STATUS_OK;
}

TreeNode *tree_get_root( const Tree *tree_ptr )
{
    assert(tree_ptr);

    return tree_ptr->root;
}

TreeNode* tree_get_left_child( const TreeNode *node_ptr )
{
    assert(node_ptr);

    return node_ptr->left;
}

TreeNode* tree_get_right_child( const TreeNode *node_ptr )
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

    if ( !is_node_leaf(node_ptr->left) )
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

    if ( !is_node_leaf(node_ptr->right) )
        return TREE_STATUS_WARNING_REQUEST_TO_DEL_NOT_A_LEAF;

    op_del_TreeNode(tree_ptr, node_ptr->right);

    node_ptr->right = NULL;

    return TREE_STATUS_OK;
}

inline TreeNode *tree_copy_node( Tree *dest, TreeNode* parent, const TreeNode *src )
{
    assert(src);

    TreeNode *node = op_new_TreeNode( dest, src->data_ptr, parent );

    if (src->left)
        node->left = tree_copy_node( dest, node, src->left );

    if (src->right)
        node->right = tree_copy_node( dest, node, src->right );

    return node;
}

TreeStatus tree_copy( Tree *dest, const Tree *src )
{
    assert(src);
    assert(dest);
    TREE_SELFCHECK(src);

#ifdef TREE_DO_DUMP
    tree_ctor(dest, src->data_size, src->typical_num_of_nodes, src->data_dtor_func_ptr, src->print_data_func_ptr);
#else /* NOT TREE_DO_DUMP */
    tree_ctor(dest, src->data_size, src->typical_num_of_nodes, src->data_dtor_func_ptr);
#endif

    if (src->root)
        dest->root = tree_copy_node( dest, NULL, src->root );

    return TREE_STATUS_OK;
}

TreeStatus tree_copy_subtree_into_left( Tree *dest, TreeNode *dest_node, const TreeNode *src_subtree)
{
    assert(dest);
    assert(dest_node);
    assert(src_subtree);

    if (dest_node->left)
        return TREE_STATUS_WARNING_LEFT_CHILD_IS_OCCUPIED;

    dest_node->left = tree_copy_node(dest, dest_node, src_subtree);

    return TREE_STATUS_OK;
}

TreeStatus tree_copy_subtree_into_right( Tree *dest, TreeNode *dest_node, const TreeNode *src_subtree)
{
    assert(dest);
    assert(dest_node);
    assert(src_subtree);

    if (dest_node->right)
        return TREE_STATUS_WARNING_LEFT_CHILD_IS_OCCUPIED;

    dest_node->right = tree_copy_node(dest, dest_node, src_subtree);

    return TREE_STATUS_OK;
}

//! @note Returns 1 if the 'except_node' is in the to be deleted subtree, 0 otherwise.
inline int del_rec( Tree *tree_ptr, TreeNode *start_with, TreeNode* except_node )
{
    assert(tree_ptr);
    assert(start_with);
    assert(except_node);

    int except_node_found = 0;

    if ( start_with->left && start_with->left != except_node )
        except_node_found |= del_rec( tree_ptr, start_with->left, except_node );
    else if ( start_with->left == except_node )
        except_node_found |= 1;

    start_with->left = NULL;

    if ( start_with->right && start_with->right != except_node )
        except_node_found |= del_rec( tree_ptr, start_with->right, except_node );
    else if ( start_with->right == except_node )
        except_node_found |= 1;
    start_with->right = NULL;

    op_del_TreeNode( tree_ptr, start_with );

    return except_node_found;
}

void tree_update_all_tree_levels( Tree *tree_ptr, TreeNode *curr_node, size_t curr_level )
{
    assert(tree_ptr);

    if (curr_node == NULL)
    {
        curr_node = tree_get_root( tree_ptr );
        if (!curr_node)
            return;
        curr_level = 0;
        tree_ptr->depth = 0;
    }

    curr_node->level = curr_level;
    if ( tree_ptr->depth < curr_level )
        tree_ptr->depth = curr_level;

    if ( curr_node->left )
        tree_update_all_tree_levels( tree_ptr, curr_node->left, curr_level + 1 );

    if ( curr_node->right)
        tree_update_all_tree_levels( tree_ptr, curr_node->right, curr_level + 1 );
}

TreeStatus tree_migrate_into_left( Tree *tree_ptr, TreeNode *dest_node, TreeNode *migr_node )
{
    TREE_SELFCHECK(tree_ptr);
    assert(dest_node);
    assert(migr_node);

    int migr_node_found = 0;
    if (dest_node->left)
        migr_node_found = del_rec( tree_ptr, dest_node->left, migr_node );

    if (!migr_node_found && migr_node->parent)
    {
        if      (migr_node->parent->left == migr_node)
            migr_node->parent->left = NULL;
        else if (migr_node->parent->right == migr_node)
            migr_node->parent->right = NULL;
    }

    dest_node->left     = migr_node;
    migr_node->parent   = dest_node;

    tree_update_all_tree_levels( tree_ptr );

    return TREE_STATUS_OK;
}

TreeStatus tree_migrate_into_right( Tree *tree_ptr, TreeNode *dest_node, TreeNode *migr_node )
{
    TREE_SELFCHECK(tree_ptr);
    assert(dest_node);
    assert(migr_node);

    int migr_node_found = 0;
    if (dest_node->right)
        migr_node_found = del_rec( tree_ptr, dest_node->right, migr_node );

    if (!migr_node_found && migr_node->parent)
    {
        if      (migr_node->parent->left == migr_node)
            migr_node->parent->left = NULL;
        else if (migr_node->parent->right == migr_node)
            migr_node->parent->right = NULL;
    }

    dest_node->right    = migr_node;
    migr_node->parent   = dest_node;

    tree_update_all_tree_levels( tree_ptr );

    return TREE_STATUS_OK;
}

TreeStatus tree_migrate_into_root( Tree *tree_ptr, TreeNode *migr_node )
{
    TREE_SELFCHECK(tree_ptr);
    assert(migr_node);

    del_rec( tree_ptr, tree_get_root(tree_ptr), migr_node );

    migr_node->parent = NULL;
    tree_ptr->root = migr_node;

    tree_update_all_tree_levels(tree_ptr);

    return TREE_STATUS_OK;
}

TreeStatus tree_delete_subtree( Tree *tree_ptr, TreeNode *subtree )
{
    TREE_SELFCHECK(tree_ptr);
    assert(subtree);

    if ( subtree->left )
    {
        TreeStatus err = tree_delete_subtree( tree_ptr, subtree->left );
        if (err)
            return err;
    }
    if ( subtree->right )
    {
        TreeStatus err = tree_delete_subtree( tree_ptr, subtree->right );
        if (err)
            return err;
    }

    op_del_TreeNode(tree_ptr, subtree);

    return TREE_STATUS_OK;
}

TreeStatus tree_hang_loose_node_at_left( Tree *tree_ptr, TreeNode *loose_node, TreeNode *parent_node )
{
    if ( parent_node->left )
        return TREE_STATUS_WARNING_LEFT_CHILD_IS_OCCUPIED;

    parent_node->left = loose_node;
    loose_node->parent = parent_node;

    tree_update_all_tree_levels( tree_ptr, loose_node, parent_node->level + 1 );

    return TREE_STATUS_OK;
}

TreeStatus tree_hang_loose_node_at_right( Tree *tree_ptr, TreeNode *loose_node, TreeNode *parent_node )
{
    if ( parent_node->right )
        return TREE_STATUS_WARNING_RIGHT_CHILD_IS_OCCUPIED;

    parent_node->right = loose_node;
    loose_node->parent = parent_node;

    tree_update_all_tree_levels( tree_ptr, loose_node, parent_node->level + 1 );

    return TREE_STATUS_OK;
}

TreeStatus tree_hang_loose_node_as_root( Tree *tree_ptr, TreeNode *loose_node)
{
    if ( tree_ptr->root )
        return TREE_STATUS_WARNING_ROOT_ALREADY_EXISTS;

    tree_ptr->root = loose_node;
    loose_node->parent = NULL;

    tree_update_all_tree_levels( tree_ptr );

    return TREE_STATUS_OK;
}


int is_node_leaf( const TreeNode* node_ptr)
{
    assert(node_ptr);

    return ( !node_ptr->left && !node_ptr->right );
}

TreeNode *op_new_TreeNode( Tree *tree_ptr, void *data, TreeNode* parent )
{
    assert(data);

    //char *new_mem = (char*) calloc( 1, sizeof(TreeNode) + tree_ptr->data_size );
    char *new_mem = (char*) _tree_alloc_new();
    if (!new_mem)
        return NULL;

    memcpy( new_mem + sizeof(TreeNode), data, tree_ptr->data_size );

    TreeNode *new_node = (TreeNode *) new_mem;
    new_node->data_ptr = (void*) (new_mem + sizeof(TreeNode));

    TreeNode *tmp = tree_ptr->head_of_all_nodes;
    tree_ptr->head_of_all_nodes = new_node;
    new_node->next = tmp;
    if (tmp)
    {
        tmp->prev = new_node;
    }

    new_node->parent = parent;

    if (parent)
        new_node->level = parent->level + 1;
    else
        new_node->level = 0;

    if (tree_ptr->depth < new_node->level)
        tree_ptr->depth = new_node->level;

    tree_ptr->nodes_count++;

    return new_node;
}

void op_del_TreeNode( Tree *tree_ptr, TreeNode *node_ptr )
{
    assert(tree_ptr);
    assert(node_ptr);

    if ( tree_ptr->data_dtor_func_ptr ) tree_ptr->data_dtor_func_ptr( node_ptr->data_ptr );

    if      ( node_ptr->parent && node_ptr->parent->left == node_ptr )
        node_ptr->parent->left = NULL;
    else if ( node_ptr->parent && node_ptr->parent->right == node_ptr )
        node_ptr->parent->right = NULL;

    node_ptr->left      = NULL;
    node_ptr->right     = NULL;
    node_ptr->data_ptr  = NULL;
    node_ptr->parent    = NULL;

    TreeNode *next = node_ptr->next;
    TreeNode *prev = node_ptr->prev;
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;

    if (!prev)
    {
       tree_ptr->head_of_all_nodes = next;
    }

    node_ptr->next = NULL;
    node_ptr->prev = NULL;

    //free(node_ptr);
    _tree_alloc_del( node_ptr );

    tree_ptr->nodes_count--;
}
