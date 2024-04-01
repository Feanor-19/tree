#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "tree_alloc.h"


typedef unsigned char byte;
struct MemPool
{
    // array of bytes, associated with this mempool
    byte *mempool = NULL;

    // index of the first elem in the linked list of free elems of the pool
    //! @note If free_elem_ind == MEM_POOLS_SIZE,
    //! it means that this memory pool is full.
    size_t free_elem_ind;
};


//! @brief Size of one block ( sizeof(TreeNode) + tree_ptr->data_size ).
static size_t BLOCK_SIZE = 0;

//! @attention Size of one MEM_POOL in BLOCKS, NOT BYTES!
static size_t MEM_POOL_SIZE = 0;

//! @brief Array of memory pools, each of size set
//! during initialization. New memory pools are
//! allocated when all previous are full.
//! @note It is supposed that there is only one memory pool,
//! and only in some extraordinary cases there might be created
//! the second or the third one.
static MemPool *MEM_POOLS = NULL;

//! @brief Current count of allocated memory pools.
static size_t MEM_POOLS_COUNT = 0;



#define ACCESS_FREE_MEM_BLOCK(mem_pool_id__, anchor__)  \
( *((size_t *) (MEM_POOLS[mem_pool_id__].mempool + anchor__*BLOCK_SIZE)) )



//! @returns true if memory pool with given id doesn't
//! have any free space, false otherwise.
inline bool is_mempool_full( size_t mem_pool_id )
{
    return ( MEM_POOLS[mem_pool_id].free_elem_ind == MEM_POOL_SIZE );
}

//! @brief links all blocks of given memory pool, sets corresponding
//! memory pool tail.
inline void init_mem_pool( size_t mem_pool_id )
{
    assert(mem_pool_id < MEM_POOLS_COUNT);

    for (size_t ind = 0; ind < MEM_POOL_SIZE; ind++)
    {
        ACCESS_FREE_MEM_BLOCK(mem_pool_id, ind) = ind + 1;
    }

    MEM_POOLS[mem_pool_id].free_elem_ind = 0;
}

TreeAllocRes _tree_alloc_init( size_t tree_node_with_data_size, size_t mem_pool_size )
{
    if ( mem_pool_size == 0 ) return TREE_ALLOC_WRONG_MEM_POOL_SIZE_TO_INIT;
    if ( MEM_POOLS_COUNT > 0 ) return TREE_ALLOC_ERR_ALREADY_INITED;

    MEM_POOLS = (MemPool*) calloc( 1, sizeof(MemPool) );
    if ( MEM_POOLS == NULL ) return TREE_ALLOC_ERR_CANT_ALLOC_MEM;

    // we are going to store size_t in free blocks, so we should align it with some 'filling'
    size_t mod = tree_node_with_data_size % sizeof(size_t);
    size_t filling = (mod == 0 ? 0 : sizeof(size_t) - mod);

    BLOCK_SIZE = tree_node_with_data_size + filling;
    MEM_POOL_SIZE = mem_pool_size;

    MEM_POOLS[0].mempool = (byte *) calloc( MEM_POOL_SIZE, BLOCK_SIZE );

    if ( MEM_POOLS[0].mempool == NULL )
    {
        free(MEM_POOLS);
        return TREE_ALLOC_ERR_CANT_ALLOC_MEM;
    }

    MEM_POOLS_COUNT++;

    init_mem_pool( 0 );

    return TREE_ALLOC_OK;
}

void* _tree_alloc_new()
{
    if ( MEM_POOLS_COUNT == 0 ) return NULL;

    bool all_memory_pools_full = true;
    size_t free_mem_pool_id = 0;
    for ( ; free_mem_pool_id < MEM_POOLS_COUNT; free_mem_pool_id++ )
    {
        if ( !is_mempool_full(free_mem_pool_id) )
        {
            all_memory_pools_full = false;
            break;
        }
    }

    //if (all_memory_pools_full) ; // TODO - create new memory pool
    // set free_mem_pool_id to id of the new mem pool
    if ( all_memory_pools_full ) 
    {
        MemPool* new_MEM_POOLS = (MemPool*) realloc( MEM_POOLS, (MEM_POOLS_COUNT+1)*sizeof(MemPool) );
        if (!new_MEM_POOLS) return NULL;
        
        MEM_POOLS = new_MEM_POOLS;
        MEM_POOLS_COUNT++;

        MEM_POOLS[MEM_POOLS_COUNT-1].mempool = (byte *) calloc( MEM_POOL_SIZE, BLOCK_SIZE );
        if ( !MEM_POOLS[MEM_POOLS_COUNT-1].mempool ) return NULL;
    
        init_mem_pool( MEM_POOLS_COUNT-1 );
    }

    size_t old_free_ptr = MEM_POOLS[ free_mem_pool_id ].free_elem_ind;
    size_t new_free_ptr = ACCESS_FREE_MEM_BLOCK( free_mem_pool_id, old_free_ptr );
    MEM_POOLS[ free_mem_pool_id ].free_elem_ind = new_free_ptr;

    void *new_mem_block_ptr = &ACCESS_FREE_MEM_BLOCK( free_mem_pool_id, old_free_ptr );
    memset( new_mem_block_ptr, 0, BLOCK_SIZE );

    ((TreeNode *) new_mem_block_ptr)->mem_pool_id = free_mem_pool_id;
    ((TreeNode *) new_mem_block_ptr)->mem_pool_anchor = old_free_ptr;

    return new_mem_block_ptr;
}

TreeAllocRes _tree_alloc_del( TreeNode *node_ptr )
{
    assert(node_ptr);

    if ( MEM_POOLS_COUNT == 0 ) return TREE_ALLOC_ERR_NOT_INITED;

    size_t mem_pool_id      = node_ptr->mem_pool_id;
    size_t mem_pool_anchor  = node_ptr->mem_pool_anchor;

    size_t old_free = MEM_POOLS[ mem_pool_id ].free_elem_ind;
    ACCESS_FREE_MEM_BLOCK( mem_pool_id, mem_pool_anchor ) = old_free;
    MEM_POOLS[ mem_pool_id ].free_elem_ind = mem_pool_anchor;

    return TREE_ALLOC_OK;
}

TreeAllocRes _tree_alloc_deinit()
{
    if ( MEM_POOLS_COUNT == 0 ) return TREE_ALLOC_ERR_ALREADY_INITED;

    for (size_t mem_pool_id = 0; mem_pool_id < MEM_POOLS_COUNT; mem_pool_id++)
    {
        free( MEM_POOLS[ mem_pool_id ].mempool );
    }
    free( MEM_POOLS );

    BLOCK_SIZE = 0;
    MEM_POOL_SIZE = 0;
    MEM_POOLS_COUNT = 0;

    return TREE_ALLOC_OK;
}
