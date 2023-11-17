#include "tree.h"

#include <stdio.h>
#include <assert.h>

TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
                       int (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       int (*data_dtor_func_ptr)(void *data_ptr)
#ifdef TREE_DO_DUMP
                       , TreeOrigInfo orig_info
#endif
                    )
{
    assert(tree_ptr);
    assert(data_size_in_bytes > 0);

    tree_ptr->data_size             = data_size_in_bytes;
    tree_ptr->print_data_func_ptr   = print_data_func_ptr;
    tree_ptr->data_dtor_func_ptr    = data_dtor_func_ptr;

#ifdef TREE_DO_DUMP
    tree_ptr->orig_info = orig_info;
#endif

    return TREE_STATUS_OK;
}

TreeStatus tree_dtor( Tree *tree_ptr )
{

}

TreeStatus tree_insert_data_as_left_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{

}

TreeStatus tree_insert_data_as_right_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{

}


#ifdef TREE_DO_DUMP
static tree_verify_t tree_verify( Tree *tree_ptr )
{

}

void tree_dump_( Tree *tree_ptr,
                 tree_verify_t verify_res,
                 const char *file,
                 const int line,
                 const char *func)
{

}

TreeStatus tree_insert_data_as_left_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{
    TREE_SELFCHECK( tree_ptr );
    assert( node_ptr );
    assert(data);


}

TreeStatus tree_insert_data_as_right_child( Tree *tree_ptr, TreeNode *node_ptr, void *data )
{

}

TreeStatus tree_get_data( Tree *tree_ptr, TreeNode *node_ptr, void *ret )
{

}

TreeNode* tree_get_left_child( Tree *tree_ptr, TreeNode *node_ptr )
{

}

TreeNode* tree_get_right_child( Tree *tree_ptr, TreeNode *node_ptr )
{

}
#endif /* TREE_DO_DUMP */
