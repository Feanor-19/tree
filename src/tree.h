#ifndef TREE_H
#define TREE_H

#include <stdint.h>

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
#define DEF_TREE_VERIFY_FLAG(name, message, cond) TREE_VERIFY_##name = bit,
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
    void *data_ptr = NULL;
    TreeNode *left = NULL;
    TreeNode *right = NULL;

#ifdef TREE_DO_DUMP
    // TreeNode *prev = NULL;
    // TreeNode *next = NULL;
#endif /* TREE_DO_DUMP */
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

    int (*print_data_func_ptr)(FILE* stream, void *data_ptr)    = NULL;
    // void * (*scan_data_func_ptr)(FILE* stream)                     = NULL;
    int (*data_dtor_func_ptr)(void *data_ptr)                   = NULL;

#ifdef TREE_DO_DUMP
    // TreeNode *head = NULL;
    // TreeNode *tail = NULL;

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
const char* const DEDLIST_DUMP_PATH = ".\\dumps\\";
const size_t DEDLIST_MAX_DUMP_PATH_LENGHT = 1024;
const size_t DEDLIST_MAX_CMD_GEN_DUMP_IMG_LENGHT = 1024;
#endif /* TREE_DO_DUMP */

//------------------------------------------------------------------------------------------

TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes
#ifdef TREE_DO_DUMP
                       , TreeOrigInfo orig_info
#endif
                    );

#ifdef TREE_DO_DUMP
#define tree_ctor( tree_ptr, data_size_in_bytes ) tree_ctor_(   tree_ptr,               \
                                                                data_size_in_bytes,     \
                                                                {                       \
                                                                    #tree_ptr,          \
                                                                    __FILE__,           \
                                                                    __LINE__,           \
                                                                    __func__            \
                                                                }                       \
                                                            )
#else /* NOT TREE_DO_DUMP */
#define tree_ctor( tree_ptr, data_size_in_bytes ) tree_ctor_(tree_ptr, data_size_in_bytes)
#endif /* TREE_DO_DUMP */

TreeStatus tree_dtor( Tree *tree_ptr );

#ifdef TREE_DO_DUMP
static tree_verify_t tree_verify( Tree *tree_ptr );

void tree_dump_( Tree *tree_ptr,
                 tree_verify_t verify_res,
                 const char *file,
                 const int line,
                 const char *func);

#define TREE_DUMP( tree_ptr, verify_res ) tree_dump_( tree_ptr,     \
                                                      verify_res,   \
                                                      __FILE__,     \
                                                      __LINE__      \
                                                      __func__ )

#define TREE_SELFCHECK( tree_ptr ) {                        \
    tree_verify_t verify_res = tree_verify( tree_ptr );     \
    if ( verify_res != 0 )                                  \
    {                                                       \
        TREE_DUMP( tree_ptr, verify_res );                  \
        return TREE_STATUS_ERROR_VERIFY;                    \
    }                                                       \
}

static int tree_verify_check_nodes_count( Tree *tree_ptr )
{
    // TODO
    return 0;
}
#else /* NOT TREE_DO_DUMP */
#define TREE_DUMP( tree_ptr, verify_res ) ((void) 0)
#define TREE_SELFCHECK( tree_ptr ) ((void) 0)
#endif /* TREE_DO_DUMP */

//! @brief Inserts data into the tree as a new node at the specified place.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] place Pointer to the field "left" or "right" of the corresponding TreeNode (see note).
//! @param [in] data Data will be copied from the place, specified by this pointer.
//! @note `place` must be pointer, returned by tree_get_left_child_as_place_to_insert()/tree_get_right_child_as_place_to_insert(),
//! or pointer to the field "root" in the struct Tree, if no nodes in the tree are present at the moment.
TreeStatus tree_insert_data( Tree *tree_ptr, TreeNode **place, void *data );

//! @brief Writes data from the specified node by pointer ret.
//! @param [in] tree_ptr Tree pointer.
//! @param [in] node Node which data must be recieved.
//! @param [out] ret Pointer by which data must be written.
TreeStatus tree_get_data( Tree *tree_ptr, TreeNode *node, void *ret );

//! @brief Returns pointer to the left child of the node.
TreeNode* tree_get_left_child_as_node( Tree *tree_ptr, TreeNode *node );

//! @brief Returns pointer to the right child of the node.
TreeNode* tree_get_right_child_as_node( Tree *tree_ptr, TreeNode *node );

//! @brief Returns pointer to the field "left" of the specified node; is used in
TreeNode** tree_get_left_child_as_place_to_insert( Tree *tree_ptr, TreeNode *node );

//! @brief Returns pointer to the left child of the node.
TreeNode** tree_get_right_child_as_place_to_insert( Tree *tree_ptr, TreeNode *node );


#endif /* TREE_H */
