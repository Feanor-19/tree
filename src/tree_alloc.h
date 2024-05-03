#ifndef TREE_ALLOC_H
#define TREE_ALLOC_H

#include "tree_common.h"


enum TreeAllocRes
{
    TREE_ALLOC_OK,
    TREE_ALLOC_WRONG_MEM_POOL_SIZE_TO_INIT,
    TREE_ALLOC_ERR_ALREADY_INITED,
    TREE_ALLOC_ERR_CANT_ALLOC_MEM,
    TREE_ALLOC_ERR_NOT_INITED
};



//! @attention ONLY FOR INTERNAL USE!
//! @brief Initializes first mem_pool and sets common for
//! all mem pools mem_pool_size, which is number
//! of 'TreeNodes with data' to be stored in the mem pool,
//! NOT number of bytes!
TreeAllocRes _tree_alloc_init( size_t tree_node_with_data_size, size_t mem_pool_size );

//! @attention ONLY FOR INTERNAL USE!
//! @brief Should replace calloc( 1, sizeof(TreeNode) + tree_ptr->data_size )
//! @return Pointer to allocated memory, or NULL if some error happened.
void* _tree_alloc_new();

//! @attention ONLY FOR INTERNAL USE!
//! @brief Frees memory, where given node_ptr is located.
TreeAllocRes _tree_alloc_del( TreeNode *node_ptr );

//! @attention ONLY FOR INTERNAL USE!
//! @brief Frees all allocated memory, after this func
//! _tree_alloc_init can be called again.
TreeAllocRes _tree_alloc_deinit();

#endif /* TREE_ALLOC_H */
