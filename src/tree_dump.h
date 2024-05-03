#ifndef TREE_DUMP_H
#define TREE_DUMP_H

#include "tree_common.h"
#include <sys/types.h>

#ifdef TREE_DO_DUMP
    const mode_t MKDIR_MODE = 0777;

    tree_verify_t tree_verify( const Tree *tree_ptr );
    void tree_print_verify_res(FILE *stream, tree_verify_t verify_res);
    void tree_dump_( const Tree *tree_ptr,
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

#else /* NOT TREE_DO_DUMP */
    #define TREE_DUMP( tree_ptr, verify_res ) ((void) 0)
    #define TREE_SELFCHECK( tree_ptr ) ((void) 0)
#endif /* TREE_DO_DUMP */

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

#endif /* TREE_DUMP_H */
