#ifndef TREE_COMMON_H
#define TREE_COMMON_H

#include <stdint.h>
#include <stdio.h>

/*
    AVAILABLE DEFINES:
    - TREE_DO_DUMP
    - TREE_ABORT_ON_DUMP - requires TREE_DO_DUMP
    - TREE_SHOW_DUMP_IMG - requires TREE_DO_DUMP
*/
#define TREE_DO_DUMP
#define TREE_SHOW_DUMP_IMG


#ifdef TREE_DO_DUMP
typedef uint64_t tree_verify_t;
#endif /* TREE_DO_DUMP */


#define DEF_TREE_STATUS(name, message) TREE_STATUS_##name,
enum TreeStatus
{
    #include "tree_statuses.h"
};
#undef DEF_TREE_STATUS

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
    void *data_ptr      = NULL;
    TreeNode *left      = NULL;
    TreeNode *right     = NULL;
    TreeNode *parent    = NULL;

    size_t level = 0;        //< Distance from the root node.

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

    size_t data_size    = 0;
    size_t nodes_count  = 0;
    size_t depth        = 0; //< max level of all nodes; if only root exists, equals 0

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


//! @brief call tree_func, returning TreeStatus, and if
// returned code isn't OK, immediately returns it.
#define WRP_RET(tree_func) {        \
    TreeStatus code = tree_func;    \
    if (code != TREE_STATUS_OK)     \
        return code;                \
}

#endif /* TREE_COMMON_H */
