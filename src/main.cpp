#include <stdio.h>

#include "tree.h"

void int_dtor(void *data);

void int_print(FILE* stream, void *data);

int main()
{
    Tree tree = {};
    tree_ctor(&tree, sizeof(int), int_dtor, int_print );

    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    tree_insert_root(&tree, &arr[0]);

    tree_insert_data_as_left_child(&tree, tree_get_root(&tree), &arr[1]);

    tree_insert_data_as_right_child(&tree, tree_get_root(&tree), &arr[2]);

    TreeNode *node = tree_get_left_child( tree_get_root( &tree ) );

    tree_insert_data_as_left_child(&tree, node, &arr[3]);

    tree_insert_data_as_right_child(&tree, node, &arr[4]);

    TREE_DUMP(&tree, 0);

    return 0;
}

void int_dtor(void *data) { data = (char*) data + 1; }

void int_print(FILE* stream, void *data)
{
    fprintf(stream, "%d", *((int*) data) );
}
