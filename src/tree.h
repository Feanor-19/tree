#ifndef TREE_H
#define TREE_H

#include "tree_common.h"
#include "tree_dump.h"

// TODO - сделать ли typedef для print_data_func_ptr?
TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
#ifdef TREE_DO_DUMP
                       void (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       TreeOrigInfo orig_info,
#endif
                       void (*data_dtor_func_ptr)(void *data_ptr) = NULL
                    );

#ifdef TREE_DO_DUMP
//! @param [in] tree_ptr Tree pointer.
//! @param [in] data_size_in_bytes Size in bytes of one element to be stored in the tree.
//! @param [in] data_dtor_func_ptr Pointer to deconstructor of elememts to be stored in the tree.
//! void data_dtor(void *data_ptr)
//! @param [in] print_data_func_ptr Pointer to function, printing element.
//! void data_print(FILE *stream, void *data_ptr).
#define tree_ctor( tree_ptr, data_size_in_bytes, data_dtor_func_ptr, print_data_func_ptr ) \
    tree_ctor_  (   tree_ptr,               \
                    data_size_in_bytes,     \
                    print_data_func_ptr,    \
                    {                       \
                        #tree_ptr,          \
                        __FILE__,           \
                        __LINE__,           \
                        __func__            \
                    },                      \
                    data_dtor_func_ptr      \
                )
#else /* NOT TREE_DO_DUMP */
//! @param [in] tree_ptr Tree pointer.
//! @param [in] data_size_in_bytes Size in bytes of one element to be stored in the tree.
//! @param [in] data_dtor_func_ptr Pointer to deconstructor of elememts to be stored in the tree.
//! void data_dtor(void *data_ptr)
#define tree_ctor( tree_ptr, data_size_in_bytes, data_dtor_func_ptr ) tree_ctor_(tree_ptr, data_size_in_bytes, data_dtor_func_ptr)
#endif /* TREE_DO_DUMP */

TreeStatus tree_dtor( Tree *tree_ptr );


//! @brief Inserts data into the empty tree as a new node. The new node becomes the root node.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] data Data will be copied from the place, specified by this pointer.
//! @note If the tree is not empty (root node already exists), error is returned.
TreeStatus tree_insert_root( Tree *tree_ptr, void *data );

//! @brief Inserts data into the tree as a new node, connected to the specified node as its left child.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] node Node, which will have the newly-created node as its left child.
//! @param [in] data Data will be copied from the place, specified by this pointer.
//! @note If left child of the specified node is occupied, error is returned and nothing is changed.
TreeStatus tree_insert_data_as_left_child( Tree *tree_ptr, TreeNode *node_ptr, void *data );

//! @brief Inserts data into the tree as a new node, connected to the specified node as its right child.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] node Node, which will have the newly-created node as its right child.
//! @param [in] data Data will be copied from the place, specified by this pointer.
//! @note If right child of the specified node is occupied, error is returned and nothing is changed.
TreeStatus tree_insert_data_as_right_child( Tree *tree_ptr, TreeNode *node_ptr, void *data );

//! @brief Writes data from the specified node by pointer ret.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] node_ptr Node which data must be recieved.
//! @param [out] ret Pointer by which data must be written.
TreeStatus tree_get_data( const Tree *tree_ptr, const TreeNode *node_ptr, void *ret );

//! @brief Returns pointer to data, stored in the given node.
void *tree_get_data_ptr( const TreeNode *node_ptr );

TreeNode *tree_get_parent( TreeNode *node_ptr );

TreeStatus tree_change_data( Tree *tree_ptr, TreeNode *node_ptr, void *new_data );

TreeNode *tree_get_root( Tree *tree_ptr );

//! @brief Returns pointer to the left child of the node.
TreeNode* tree_get_left_child( TreeNode *node_ptr );

//! @brief Returns pointer to the right child of the node.
TreeNode* tree_get_right_child( TreeNode *node_ptr );

TreeStatus tree_delete_root( Tree *tree_ptr );

//! @brief Deletes specified child of the node if it is a leaf, returns errors and does nothing otherwise.
TreeStatus tree_delete_left_child( Tree *tree_ptr, TreeNode *node_ptr );

//! @brief Deletes specified child of the node if it is a leaf, returns errors and does nothing otherwise.
TreeStatus tree_delete_right_child( Tree *tree_ptr, TreeNode *node_ptr );

//! @brief Makes full copy of given tree 'source' and puts it by 'dest' pointer.
TreeStatus tree_copy( Tree *dest, const Tree *source );

//! @brief Copies full subtree, in which 'src_subtree' plays role of the root,
//! and inserts it as the left child of 'dest_node', belonging to tree 'dest'.
TreeStatus tree_copy_subtree_into_left( Tree *dest, TreeNode *dest_node, const TreeNode *src_subtree);

//! @brief Copies full subtree, in which 'src_subtree' plays role of the root,
//! and inserts it as the right child of 'dest_node', belonging to tree 'dest'.
TreeStatus tree_copy_subtree_into_right( Tree *dest, TreeNode *dest_node, const TreeNode *src_subtree);

//! @brief Returns 1 if node has no children, 0 otherwise.
int is_node_leaf( TreeNode* node_ptr);

#endif /* TREE_H */
