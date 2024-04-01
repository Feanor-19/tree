#include <stdio.h>

#include "tree.h"

void int_dtor(void *data);

void int_print(FILE* stream, void *data);

int main()
{
    // TODO - ничего из _TREE_ALLOC не протестировано!!!!!!!

    Tree tree = {};
    tree_ctor(&tree, sizeof(int), 20, int_dtor);

    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    TreeNode *node1 = op_new_TreeNode( &tree, &arr[1] );

    TreeNode *node3 = op_new_TreeNode( &tree, &arr[3] );

    tree_hang_loose_node_at_left( &tree, node1, node3 );

    TreeNode *node2 = op_new_TreeNode( &tree, &arr[2] );

    tree_hang_loose_node_at_right( &tree, node2, node3 );

    TREE_DUMP(&tree, 0);

    tree_delete_right_child( &tree, node3 );

    TREE_DUMP(&tree, 0);

    TreeNode *node4 = op_new_TreeNode( &tree, &arr[4] );

    TreeNode *node5 = op_new_TreeNode( &tree, &arr[5] );

    tree_hang_loose_node_at_right( &tree, node5, node4 );

    TreeNode *node6 = op_new_TreeNode( &tree, &arr[6] );

    tree_hang_loose_node_at_left( &tree, node6, node5 );

    // tree_hang_loose_node_at_left( &tree, node3, node4 );

    // tree_hang_loose_node_as_root( &tree, node4 );

    TREE_DUMP(&tree, 0);

    tree_delete_subtree( &tree, node5 );

    TREE_DUMP(&tree, 0);

    tree_dtor(&tree);

    printf("all is ok\n");

    return 0;
}

void int_dtor(void *data) { data = (char*) data + 1; }

void int_print(FILE* stream, void *data)
{
    fprintf(stream, "%d", *((int*) data) );
}
