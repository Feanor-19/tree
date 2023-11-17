#include "tree.h"

#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static TreeNode *op_new_TreeNode( Tree *tree_ptr, void *data);

static void op_del_TreeNode( Tree *tree_ptr, TreeNode *node_ptr );

static void tree_print_verify_res(FILE *stream, tree_verify_t verify_res);

static tree_verify_t tree_verify( Tree *tree_ptr );


TreeStatus tree_ctor_( Tree *tree_ptr,
                       size_t data_size_in_bytes,
#ifdef TREE_DO_DUMP
                       void (*print_data_func_ptr)(FILE* stream, void *data_ptr),
                       TreeOrigInfo orig_info,
#endif
                       void (*data_dtor_func_ptr)(void *data_ptr)
                    )
{
    assert(tree_ptr);
    assert(data_size_in_bytes > 0);

    tree_ptr->data_size             = data_size_in_bytes;
    tree_ptr->data_dtor_func_ptr    = data_dtor_func_ptr;
    tree_ptr->nodes_count = 0;
    tree_ptr->root = NULL;

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
        TreeNode *tmp = curr_node_ptr->next;
        op_del_TreeNode(tree_ptr, curr_node_ptr);
        curr_node_ptr = tmp;
    }

    tree_ptr->root                  = NULL;
    tree_ptr->nodes_count           = 0;
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

TreeNode* tree_get_left_child( TreeNode *node_ptr )
{
    assert(node_ptr);

    return node_ptr->left;
}

TreeNode* tree_get_right_child( TreeNode *node_ptr )
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

int is_node_leaf( TreeNode* node_ptr)
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
    new_node->next = tmp;
    if (tmp)
    {
        tmp->prev = new_node;
    }

    tree_ptr->nodes_count++;

    return new_node;
}

static void op_del_TreeNode( Tree *tree_ptr, TreeNode *node_ptr )
{
    assert(tree_ptr);
    assert(node_ptr);

    if ( tree_ptr->data_dtor_func_ptr ) tree_ptr->data_dtor_func_ptr( node_ptr->data_ptr );

    node_ptr->left = NULL;
    node_ptr->right = NULL;
    node_ptr->data_ptr = NULL;

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

    free(node_ptr);

    tree_ptr->nodes_count--;
}

#ifdef TREE_DO_DUMP

static tree_verify_t tree_verify( Tree *tree_ptr )
{
    // TODO -

    return 0;
}

static void tree_print_verify_res(FILE *stream, tree_verify_t verify_res)
{
    fprintf(stream, "Tree verification result: <%llu>\n", verify_res);
    for (size_t ind = 0; ind < sizeof(tree_verification_messages)/sizeof(tree_verification_messages[0]); ind++)
    {
        if (verify_res & ( 1 << ind ))
        {
            fprintf(stream, "----> %s\n", tree_verification_messages[ind]);
        }
    }
}

void tree_print_status_message( FILE *stream, TreeStatus status )
{
    fprintf(stream, "%s", tree_status_messages[status]);
}

inline size_t str_insert(  char *str_dest_begin,
                    size_t str_dest_capacity,
                    const char *str_source,
                    size_t start_ind,
                    int *res )
{
    assert(str_dest_begin);
    assert(res);
    assert(str_source);

    size_t cnt = 0;
    int source_copied = 0;
    while ( cnt + start_ind < str_dest_capacity - 1 ) // leaving space for '\0'
    {
        str_dest_begin[cnt + start_ind] = str_source[cnt];

        cnt++;

        if ( str_source[cnt] == '\0' )
        {
            source_copied = 1;
            break;
        }

    }

    str_dest_begin[str_dest_capacity] = '\0';

    if ( source_copied )
        *res = 1;
    else
        *res = 0;

    return cnt + start_ind;
}

//! @details Prints current local time into string stream.
//! Is string's capacity is exceeded, returns 0. If ok, returns 1.
inline int get_as_str_curr_local_time_(char *stream, size_t capacity )
{
    time_t curr_time = time(NULL);
    tm curr_local_time = *localtime(&curr_time);

    int cnt = snprintf( stream, capacity, "%d-%02d-%02d_%02d-%02d-%02d",
                        curr_local_time.tm_year + 1900,
                        curr_local_time.tm_mon + 1,
                        curr_local_time.tm_mday,
                        curr_local_time.tm_hour,
                        curr_local_time.tm_min,
                        curr_local_time.tm_sec);

    if (cnt > 0 && cnt < (int) capacity)
        return 1;
    else
        return 0;
}

inline TreeStatus create_dump_folder_( char **curr_dump_dir_ptr )
{
    assert(curr_dump_dir_ptr);

    char *dir_name = (char*) calloc(TREE_MAX_DUMP_PATH_LENGHT, sizeof(char));

    int res = 1;
    size_t curr_dir_name_ind = str_insert(  dir_name,
                                            TREE_MAX_DUMP_PATH_LENGHT,
                                            TREE_DUMP_PATH,
                                            0,
                                            &res);
    if ( !res )
        return TREE_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;

    char curr_time[TREE_MAX_DUMP_PATH_LENGHT] = "";
    if ( !get_as_str_curr_local_time_(curr_time, TREE_MAX_DUMP_PATH_LENGHT) )
    {
        assert(0 && "Too short string for curr_local_time! Please chahge it!");
    }

    curr_dir_name_ind = str_insert( dir_name,
                                    TREE_MAX_DUMP_PATH_LENGHT,
                                    curr_time,
                                    curr_dir_name_ind,
                                    &res);
    if ( !res )
        return TREE_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;


    curr_dir_name_ind = str_insert( dir_name,
                                    TREE_MAX_DUMP_PATH_LENGHT,
                                    "\\",
                                    curr_dir_name_ind,
                                    &res);
    if ( !res )
        return TREE_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;

    struct stat st = {};

    if (stat(TREE_DUMP_PATH, &st) == -1) {
        int res_dir = mkdir(TREE_DUMP_PATH);

        if (res_dir != 0)
            return TREE_STATUS_ERROR_CANT_CREATE_DUMP_DIR;
    }

    if (stat(dir_name, &st) == -1) {
        int res_dir = mkdir(dir_name);

        if (res_dir != 0)
            return TREE_STATUS_ERROR_CANT_CREATE_DUMP_DIR;
    }

    *curr_dump_dir_ptr = dir_name;

    return TREE_STATUS_OK;
}

//! @brief Pointer to newly created file is stored in *ret_ptr
inline TreeStatus create_tmp_dot_file_( const char *dot_file_path, FILE **ret_ptr )
{
    assert(dot_file_path);
    assert(ret_ptr);

    *ret_ptr = fopen( dot_file_path, "w" );
    if ( !(*ret_ptr) )
        return TREE_STATUS_ERROR_CANT_OPEN_DUMP_FILE;

    return TREE_STATUS_OK;
}

inline TreeStatus write_dot_file_for_dump_( FILE *dot_file,
                                            Tree *tree_ptr,
                                            tree_verify_t verify_res,
                                            const char *called_from_file,
                                            const int called_from_line,
                                            const char *called_from_func )
{
    // писать сразу в файл медленно, но когда генерится дамп, эта скорость не важна

#define COLOR_BG            "#2D4059"
#define COLOR_NODE_COLOR    "#ECC237"
#define COLOR_NODE_FILL     "#EA5455"
#define COLOR_LABEL_COLOR   "#EA5455"
#define COLOR_LABEL_FILL    "#ECC237"
#define COLOR_EDGE_LEFT     "#F07B3F"
#define COLOR_EDGE_RIGHT    "#FFD460"
#define COLOR_EDGE          "#8ccb5e"


    // Dot header
    fprintf(dot_file,   "digraph{\n"
                        "splines=ortho;\n"
                        "bgcolor=\"" COLOR_BG "\";"
                        "\n\n\n");


    // Node text
    fprintf(dot_file,   "NODE_TEXT[shape=note, fontname=\"verdana\",\n"
                        "style=bold, style=filled,\n"
                        "color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\",\n"
                        "label = \"");
    fprintf(dot_file,   "Tree[%p] (%s) declared in %s(%d), in function %s.\\n"
                        "TREE_DUMP() called from %s(%d), from function %s.\\n"
                        "data_size: %llu; nodes_count: %llu;\\nroot: [%p];head_of_all_nodes: [%p].\\n",
                        tree_ptr,
                        tree_ptr->orig_info.name,
                        tree_ptr->orig_info.orig_file_name,
                        tree_ptr->orig_info.orig_line,
                        tree_ptr->orig_info.orig_func_name,
                        called_from_file,
                        called_from_line,
                        called_from_func,
                        tree_ptr->data_size,
                        tree_ptr->nodes_count,
                        tree_ptr->root,
                        tree_ptr->head_of_all_nodes);
    tree_print_verify_res(dot_file, verify_res);
    fprintf(dot_file, "\"]\n\n\n");


    // Nodes with data
    TreeNode *curr_node = tree_ptr->head_of_all_nodes;
    TreeNode **nodes_arr = (TreeNode**) calloc( tree_ptr->nodes_count, sizeof(TreeNode) );
    for (size_t ind = 0; ind < tree_ptr->nodes_count; ind++)
    {
        if ( curr_node == NULL )
        {
            fprintf(stderr, "ERROR: something wrong with listing of nodes!\n");
            break;
        }

        fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                            "style=bold, style=filled,\n"
                            "color=\"" COLOR_NODE_COLOR "\", fillcolor=\"" COLOR_NODE_FILL "\",\n"
                            "label = <<table cellspacing=\"0\">\n"
                            "<tr><td colspan=\"2\">address: [%p]</td></tr>\n"
                            "<tr><td colspan=\"2\">data: ",
                            ind,
                            curr_node);
        tree_ptr->print_data_func_ptr(dot_file, curr_node->data_ptr);
        fprintf(dot_file,   "</td></tr>\n"
                            "<tr><td>left: [%p]</td><td>right: [%p]</td></tr></table>>];\n\n",
                            curr_node->left,
                            curr_node->right);

        nodes_arr[ind] = curr_node;
        curr_node = curr_node->next;
    }


    // Edges
    for (size_t ind = 0; ind < tree_ptr->nodes_count; ind++)
    {
        size_t left = 0;
        size_t right = 0;
        for (size_t i = 0; i < tree_ptr->nodes_count; i++)
        {
            if ( nodes_arr[i] == nodes_arr[ind]->left )
            {
                left = i;
            }
            else if ( nodes_arr[i] == nodes_arr[ind]->right )
            {
                right = i;
            }
        }

        if ( nodes_arr[ind]->left )
        {
            fprintf(dot_file,   "NODE_%llu->NODE_%llu[color=\"" COLOR_EDGE_LEFT "\", penwidth=2];\n",
                                ind, left);
        }

        if ( nodes_arr[ind]->right )
        {
            fprintf(dot_file,   "NODE_%llu->NODE_%llu[color=\"" COLOR_EDGE_RIGHT "\", penwidth=2];\n",
                                ind, right);
        }

        if ( nodes_arr[ind]->left && nodes_arr[ind]->right )
        {
            fprintf(dot_file,   "NODE_%llu->NODE_%llu[style=invis];\n"
                                "{rank=same NODE_%llu NODE_%llu}",
                                left, right, left, right);
        }
    }


    fprintf(dot_file, "\n}\n");


#undef COLOR_BG
#undef COLOR_OCCUP_NODE_COLOR
#undef COLOR_OCCUP_NODE_FILL
#undef COLOR_FREE_NODE_COLOR
#undef COLOR_FREE_NODE_FILL
#undef COLOR_LABEL_COLOR
#undef COLOR_LABEL_FILL
#undef COLOR_EDGE_PREV
#undef COLOR_EDGE_NEXT
#undef COLOR_EDGE_FREE


    return TREE_STATUS_OK;
}

inline TreeStatus generate_dump_img_( const char * dump_dot_path, const char * dump_img_path )
{
    char cmd[TREE_MAX_CMD_GEN_DUMP_IMG_LENGHT] = {};
    int written_chars = snprintf(   cmd,
                                    TREE_MAX_CMD_GEN_DUMP_IMG_LENGHT,
                                    "dot %s -Tjpg -o %s",
                                    dump_dot_path,
                                    dump_img_path);

    if ( written_chars >= (int) TREE_MAX_CMD_GEN_DUMP_IMG_LENGHT)
    {
        fprintf(stderr, "Command to generate dump image is too long, can't execute it. "
        "Please, make paths to files shorter.\n");
        return TREE_STATUS_ERROR_TOO_LONG_CMD_GEN_DUMP_IMG;
    }

    system(cmd);

    fprintf(stderr, "Dedlist dump image is generated!\n");

    return TREE_STATUS_OK;
}

#ifdef TREE_SHOW_DUMP_IMG
inline TreeStatus show_dump_img_( const char * dump_img_path )
{
    system(dump_img_path);

    return TREE_STATUS_OK;
}
#endif

inline TreeStatus free_dot_file_( FILE * dot_tmp_file )
{
    fclose(dot_tmp_file);

    return TREE_STATUS_OK;
}

void tree_dump_( Tree *tree_ptr,
                 tree_verify_t verify_res,
                 const char *file,
                 const int line,
                 const char *func)
{
    FILE *dot_file = NULL;

    char *curr_dump_dir = NULL;
    WRP_PRINT( create_dump_folder_(&curr_dump_dir) );

    char dot_file_path[TREE_MAX_DUMP_PATH_LENGHT] = "";
    strcpy(dot_file_path, curr_dump_dir);
    strcat(dot_file_path, "dmp.dot");

    WRP_PRINT( create_tmp_dot_file_( dot_file_path, &dot_file ) );

    WRP_PRINT( write_dot_file_for_dump_(dot_file, tree_ptr, verify_res, file, line, func ) );

    WRP_PRINT( free_dot_file_(dot_file) );

    char img_file_path[TREE_MAX_DUMP_PATH_LENGHT] = "";
    strcpy(img_file_path, curr_dump_dir);
    strcat(img_file_path, "dmp.jpg");

    WRP_PRINT( generate_dump_img_( dot_file_path, img_file_path ) );

#ifdef TREE_SHOW_DUMP_IMG
    WRP_PRINT( show_dump_img_( img_file_path ) );
#endif

    free(curr_dump_dir);

#ifdef TREE_ABORT_ON_DUMP
    abort();
#endif
}


#endif /* TREE_DO_DUMP */
