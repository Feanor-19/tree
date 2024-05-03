//DSL
#define _TREE_PTR (tree_ptr)


// VERIFY FLAGS
DEF_TREE_VERIFY_FLAG
(
    NULL_TREE_PNT,
    "NULL_TREE_PNT",
    (!_TREE_PTR)
)

DEF_TREE_VERIFY_FLAG
(
    WRONG_HEAD_OR_NODES_COUNT,
    "WRONG_HEAD_OR_NODES_COUNT",
    (_TREE_PTR &&
    ((_TREE_PTR->nodes_count == 0 && _TREE_PTR->head_of_all_nodes)
  || (_TREE_PTR->nodes_count != 0 && !_TREE_PTR->head_of_all_nodes)) )
)

DEF_TREE_VERIFY_FLAG
(
    INVALID_NODES_COUNT_ACCORDING_TO_LIST,
    "INVALID_NODES_COUNT_ACCORDING_TO_LIST",
    (_TREE_PTR &&
    !verify_check_nodes_count(_TREE_PTR))
)

DEF_TREE_VERIFY_FLAG
(
    SOME_NODES_HAVE_NULL_DATA_POINTER,
    "SOME_NODES_HAVE_NULL_DATA_POINTER",
    (_TREE_PTR &&
    !verify_check_nodes_data_pointer(_TREE_PTR))
)


//UNDEF DSL
#undef _TREE_PTR
