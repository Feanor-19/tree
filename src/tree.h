#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <stdio.h>

/*
    AVAILABLE DEFINES:
    - TREE_DO_DUMP
    - TREE_ABORT_ON_DUMP - requires TREE_DO_DUMP
    - TREE_SHOW_DUMP_IMG - requires TREE_DO_DUMP

    Примечание о переработке:
    - дефайны (TREE_DO_DUMP и т.д.) остаются, чтобы при желании скомпилить
      другой вариант библиотеки, достаточно было их просто переключить


*/
#define TREE_DO_DUMP
#define TREE_SHOW_DUMP_IMG

//------------------------------------------------------------------------------------------

typedef uint64_t tree_verify_t;

//------------------------------------------------------------------------------------------

#define DEF_TREE_STATUS(name, message) TREE_STATUS_##name,
enum TreeStatus
{
    #include "tree_statuses.h"
};
#undef DEF_TREE_STATUS
// TODO - дописать верификатор!!!
#ifdef TREE_DO_DUMP
#define DEF_TREE_VERIFY_FLAG(name, message, cond) TREE_VERIFY_##name,
enum TreeVerifyFlag
{
    #include "tree_verify_flags.h"
};
#undef DEF_TREE_VERIFY_FLAG
#endif /* TREE_DO_DUMP */

//! @note data_ptr points at the memory block right after
//! where TreeNode is located itself. Number of bytes to read as data
//! is stored in the Tree struct.
struct TreeNode
{
    // TODO - добавить ранг (расстояние от корня)
    void *data_ptr = NULL;
    TreeNode *left = NULL;
    TreeNode *right = NULL;

    TreeNode *prev = NULL;  //< Is used in tree_dtor() and tree_dump()
    TreeNode *next = NULL;  //< Is used in tree_dtor() and tree_dump()
};

#ifdef TREE_DO_DUMP
struct TreeOrigInfo
{
    const char *name = NULL;
    const char *orig_file_name = NULL;
    int orig_line = -1;
    const char *orig_func_name = NULL;
};
#endif /* TREE_DO_DUMP */

struct Tree
{
    TreeNode *root = NULL;

    size_t data_size = 0;
    size_t nodes_count = 0;

    void (*data_dtor_func_ptr)(void *data_ptr) = NULL;

    TreeNode *head_of_all_nodes = NULL; //< Is used in tree_dtor() and tree_dump()

#ifdef TREE_DO_DUMP
    void (*print_data_func_ptr)(FILE* stream, void *data_ptr) = NULL;
    TreeOrigInfo orig_info = {};
#endif /* TREE_DO_DUMP */
};

#define DEF_TREE_STATUS(name, message) message,
const char * const tree_status_messages[] =
{
    #include "tree_statuses.h"
    "FICTIONAL MESSAGE!"
};
#undef DEF_TREE_STATUS

#ifdef TREE_DO_DUMP
#define DEF_TREE_VERIFY_FLAG(name, message, cond) message,
const char * const tree_verification_messages[] =
{
    #include "tree_verify_flags.h"
};
#undef DEF_TREE_VERIFY_FLAG

//! @brief Dump files will be stored in folder, specified by this path.
const char* const TREE_DUMP_PATH = ".\\dumps\\";
const size_t TREE_MAX_DUMP_PATH_LENGHT = 1024;
const size_t TREE_MAX_CMD_GEN_DUMP_IMG_LENGHT = 1024;
#endif /* TREE_DO_DUMP */

//------------------------------------------------------------------------------------------

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
// TODO - написать пояснение
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
#define tree_ctor( tree_ptr, data_size_in_bytes, data_dtor_func_ptr ) tree_ctor_(tree_ptr, data_size_in_bytes, data_dtor_func_ptr)
#endif /* TREE_DO_DUMP */

TreeStatus tree_dtor( Tree *tree_ptr );

#ifdef TREE_DO_DUMP
// TODO - добавить в верификатор проверку на то, что все узлы имеют данные

void tree_dump_( Tree *tree_ptr,
                 tree_verify_t verify_res,
                 const char *file,
                 const int line,
                 const char *func);

#define TREE_DUMP( tree_ptr, verify_res ) tree_dump_( tree_ptr,     \
                                                      verify_res,   \
                                                      __FILE__,     \
                                                      __LINE__,     \
                                                      __func__ )


#define TREE_SELFCHECK( tree_ptr ) {                        \
    tree_verify_t verify_res = tree_verify( tree_ptr );     \
    if ( verify_res != 0 )                                  \
    {                                                       \
        TREE_DUMP( tree_ptr, verify_res );                  \
        return TREE_STATUS_ERROR_VERIFY;                    \
    }                                                       \
}

void tree_print_status_message( FILE *stream, TreeStatus status );

// static int tree_verify_check_nodes_count( Tree *tree_ptr );

#else /* NOT TREE_DO_DUMP */
#define TREE_DUMP( tree_ptr, verify_res ) ((void) 0)
#define TREE_SELFCHECK( tree_ptr ) ((void) 0)
#endif /* TREE_DO_DUMP */

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
TreeStatus tree_get_data( Tree *tree_ptr, TreeNode *node_ptr, void *ret );

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

//! @brief Returns 1 if node has no children, 0 otherwise.
int is_node_leaf( TreeNode* node_ptr);

//! @brief call tree_func, returning TreeStatus, and if
// returned code isn't OK, immediately returns it.
#define WRP_RET(tree_func) {        \
    TreeStatus code = tree_func;    \
    if (code != TREE_STATUS_OK)     \
        return code;                \
}

//! @brief call tree_func, returning TreeStatus, and if
// returned code isn't OK, prints status code and returns void.
#define WRP_PRINT(tree_func) {                          \
    TreeStatus code = tree_func;                        \
    if (code != TREE_STATUS_OK)                         \
    {                                                   \
        tree_print_status_message(stderr, code);        \
        return;                                         \
    }                                                   \
}

#endif /* TREE_H */
