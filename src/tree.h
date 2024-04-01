#ifndef TREE_H
#define TREE_H

#include "tree_common.h"
#include "tree_dump.h"

TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
#ifdef TREE_DO_DUMP
                       void (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       TreeOrigInfo orig_info,
#endif
                       size_t typical_num_of_nodes,
                       void (*data_dtor_func_ptr)(void *data_ptr) = NULL
                    );

#ifdef TREE_DO_DUMP
//! @param [in] tree_ptr Tree pointer.
//! @param [in] data_size_in_bytes Size in bytes of one element to be stored in the tree.
//! @param [in] data_dtor_func_ptr Pointer to deconstructor of elememts to be stored in the tree.
//! void data_dtor(void *data_ptr)
//! @param [in] typical_num_of_nodes Expected maximum number of nodes in the tree (might be exceeded).
//! @param [in] print_data_func_ptr Pointer to function, printing element.
//! void data_print(FILE *stream, void *data_ptr).
#define tree_ctor( tree_ptr, data_size_in_bytes, typical_num_of_nodes, data_dtor_func_ptr, print_data_func_ptr ) \
    tree_ctor_  (   tree_ptr,               \
                    data_size_in_bytes,     \
                    print_data_func_ptr,    \
                    {                       \
                        #tree_ptr,          \
                        __FILE__,           \
                        __LINE__,           \
                        __func__            \
                    },                      \
                    typical_num_of_nodes,   \
                    data_dtor_func_ptr      \
                )
#else /* NOT TREE_DO_DUMP */
//! @param [in] tree_ptr Tree pointer.
//! @param [in] data_size_in_bytes Size in bytes of one element to be stored in the tree.
//! @param [in] typical_num_of_nodes Expected maximum number of nodes in the tree (might be exceeded).
//! @param [in] data_dtor_func_ptr Pointer to deconstructor of elememts to be stored in the tree.
//! void data_dtor(void *data_ptr)
#define tree_ctor( tree_ptr__, data_size_in_bytes__, typical_num_of_nodes__, data_dtor_func_ptr__ ) \
    tree_ctor_(tree_ptr__, data_size_in_bytes__, typical_num_of_nodes__, data_dtor_func_ptr__ )
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

TreeNode *tree_get_parent( const TreeNode *node_ptr );

TreeStatus tree_change_data( Tree *tree_ptr, TreeNode *node_ptr, void *new_data );

TreeNode *tree_get_root( const Tree *tree_ptr );

//! @brief Returns pointer to the left child of the node.
TreeNode* tree_get_left_child( const TreeNode *node_ptr );

//! @brief Returns pointer to the right child of the node.
TreeNode* tree_get_right_child( const TreeNode *node_ptr );

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

//! @brief Hangs the subtree, which starts with 'migr_node', as the left child of the 'dest_node'.
//! @note Deletes the whole left subtree of the 'dest_node'! BUT, the 'migr_node' is allowed
//! to be located in the left subtree of the 'dest_node'.
//! @note ATTENTION! 'migr_node' and 'dest_node' must belong to the same tree!
//! @note ATTENTION! 'dest_node' MUST NOT BE LOCATED IN ANY OF 'migr_node' SUBTREES!
TreeStatus tree_migrate_into_left( Tree *tree_ptr, TreeNode *dest_node, TreeNode *migr_node );

//! @brief Hangs the subtree, which starts with 'migr_node', as the right child of the 'dest_node'.
//! @note Deletes the whole right subtree of the 'dest_node'! BUT, the 'migr_node' is allowed
//! to be located in the right subtree of the 'dest_node'.
//! @note ATTENTION! 'migr_node' and 'dest_node' must belong to the same tree!
//! @note ATTENTION! 'dest_node' MUST NOT BE LOCATED IN ANY OF 'migr_node' SUBTREES!
TreeStatus tree_migrate_into_right( Tree *tree_ptr, TreeNode *dest_node, TreeNode *migr_node );

//! @brief The whole tree is replaced with the subtree, which starts with 'migr_node'.
TreeStatus tree_migrate_into_root( Tree *tree_ptr, TreeNode *migr_node );

TreeStatus tree_delete_subtree( Tree *tree_ptr, TreeNode *subtree );

//! @brief Hangs specified 'loose_node' as the left child of the 'parent_node'.
//! @note ATTENTION: 'loose_node' must be created using 'op_new_TreeNode' for the same tree
//! and it mustn't be a child of any other node in the tree!
TreeStatus tree_hang_loose_node_at_left( Tree *tree_ptr, TreeNode *loose_node, TreeNode *parent_node );

//! @brief Hangs specified 'loose_node' as the right child of the 'parent_node'.
//! @note ATTENTION: 'loose_node' must be created using 'op_new_TreeNode' for the same tree
//! and it mustn't be a child of any other node in the tree!
TreeStatus tree_hang_loose_node_at_right( Tree *tree_ptr, TreeNode *loose_node, TreeNode *parent_node );

//! @brief Hangs specified 'loose_node' as the root of the tree.
//! @note ATTENTION: 'loose_node' must be created using 'op_new_TreeNode' for the same tree
//! and it mustn't be a child of any other node in the tree!
TreeStatus tree_hang_loose_node_as_root( Tree *tree_ptr, TreeNode *loose_node);

void tree_update_all_tree_levels( Tree *tree_ptr, TreeNode *curr_node = NULL, size_t curr_level = 0 );

//! @note ATTENTION: USE ONLY IF YOU DO KNOW WHAT YOU ARE DOING!
TreeNode *op_new_TreeNode( Tree *tree_ptr, void *data, TreeNode* parent = NULL);

//! @note ATTENTION: USE ONLY IF YOU DO KNOW WHAT YOU ARE DOING!
void op_del_TreeNode( Tree *tree_ptr, TreeNode *node_ptr );

//! @brief Returns 1 if node has no children, 0 otherwise.
int is_node_leaf( const TreeNode* node_ptr);

#endif /* TREE_H */
