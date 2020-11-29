
#include <adts.h>
#include <utest.h>


/******************************************************************************
 #####  ####### ######  #     #  #####  ####### #     # ######  #######  #####
#     #    #    #     # #     # #     #    #    #     # #     # #       #     #
#          #    #     # #     # #          #    #     # #     # #       #
 #####     #    ######  #     # #          #    #     # ######  #####    #####
      #    #    #   #   #     # #          #    #     # #   #   #             #
#     #    #    #    #  #     # #     #    #    #     # #    #  #       #     #
 #####     #    #     #  #####   #####     #     #####  #     # #######  #####
******************************************************************************/
typedef struct list_elem {
    uint32_t           value;
    struct list_elem  *next;
} list_elem_t;


/******************************************************************************
   ####### #     # #     #  #####  #######   ###   ####### #     #  #####
   #       #     # ##    # #     #    #       #    #     # ##    # #     #
   #       #     # # #   # #          #       #    #     # # #   # #
   #####   #     # #  #  # #          #       #    #     # #  #  #  #####
   #       #     # #   # # #          #       #    #     # #   # #       #
   #       #     # #    ## #     #    #       #    #     # #    ## #     #
   #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/
/*
 ****************************************************************************
 * Q: Write code to remove duplicates from unsorted linked list.
 *
 ****************************************************************************
 */
#if 0
Questions:
- List of what? strings
- Range of data? random
- Single or double linked list? single
- Number of elements? unknown
- Memory constraints? RAM
- Duplicate node or duplicate data? Data

Tests:
NULL list
Single element list
N element list

Proposal
If buffer allowed
Hash T=O(n), S=O(n-1)
If no temporal buffer
Sort in place via

Complexity:
See above

Pseudo

    Walk list add to hash.  On add detect dup, if dup drop from list.
    Return clean list

Code:

typedef struct list_elem {
    char                  *string;
    struct list_elem *p_next;
} list_elem_t;

typedef struct ht_elem {
    char   *data;
    Size_t dups;
} ht_elem_t;

typedef struct hash_table {
Int32_t       entries;
    size_t         table_bytes;
    Ht_elem_t *arr;  //arr[]
} hash_table_t;

//create a hashtable subfunc here.
//ht_destroy() -
hash_table_t
ht_create( size_t entries )
{
Bool               rc = -1;
    hash table_t p_ht = NULL;
    Size_t           abytes = entries * sizeof(hash_table_t);

    P_ht = calloc(1, sizeof(*p_ht));
If (NULL == p_ht) {
    Rc = -1;
    Goto done;
}

p_ht->arr = calloc(1, abytes);
If (NULL == p_ht->arr) {
    Rc = -1;
    Goto done;
}

Done:
    If (rc) {
        If (p_ht && p_ht->arr) {
            free(p_ht->arr);
}

If (p_ht) {
    free(p_ht);
}
}
return p_ht;
}

static bool
is_dup( list_elem_t *p_elem )
{
bool rc = false;
Uint64_t index = 0;


Index = (p_elem->data % p_ht->entries);
If (p_ht->arr[index].data == p_elem->data) {
    //dup
    Rc = true;
Goto done;
}else {
    //collision
    //Add to list on index.

}

done:
    return rc;
} //is_dup()

List_elem_t *
dup_clean( list_elem_t *p_elem)
{
    List_elem_t    *p_head = p_elem;
    Hash_table_t *p_ht      = NULL;

    //sanity
    If (NULL == p_elem) {
        goto done;
    }

    p_ht = ht_create( estimate / 2);

    // walk list
    While (p_elem) {
        If (is_dup(p_elem)) {
            // skip over the dup
            p_elem = p_elem->p_next->p_next;
} else {
        p_elem = p_elem->next;
}
    }

done:
    // destroy hashtable

return p_head;
} //dup_clean()

#endif

/*
 ****************************************************************************
 * Q: Write code to remove duplicates from unsorted linked list.
 *    - How would you solve this problem is a temporary buffer is not allowed.
 *
 ****************************************************************************
 */
#if 0
Tests:
Null
Single
Valid list

Complexity:  T=O(n^2), S=O(1)






Static void
dup_drops( list_elem_t *p_elem,
const chat *data )
{
    list_elem_t  *p_fast = p_elem;

While (p_fast) {
        If (strcmp(data, p_fast->data)) {
            //drop
            If (p_fast->next) {
            P_fast = p_fast->next->next;
}else {
            P_fast = p_fast->next;
}
}
}

    return;
} // dup_drops()





List_elem_t *
dup_clean( list_elem_t *p_elem )
{
    List_elem_t *p_head = p_elem;
    List_elem *p_slow;

    If (NULL == p_elem) {
        Goto done;
}

If (NULL == p_elem->next) {
    Goto done;
}

    p_slow = p_elem;

    While (p_elem) {
(void) dup_drop(p_slow->data, p_slow->next;
    P_slow = p_slow->next->next;
}

done:
    Return p_head;
} // dup_clean()

#endif


/*
 ****************************************************************************
 * Q: kth to last in singly linked list
 *
 ****************************************************************************
 */
#if 0
Kth to last in singly linked list

What is the size of the list?

Tests:

Complexity:
    T=O(n), S=O(1)

List_elem_t *
kth_elem( list_elem_t *p_elem,
                 Uint32_t      kth )
{
    Uint64_t      cnt = 0;
    List_elem_t p_kth = NULL;
    List_elem_t p_fast = p_elem;

    While (p_fast) {
        If (cnt >= kth) {
            P_kth = p_fast;
}
cnt++
        P_fast - p_fast->next;
}

    Return p_kth;
} //kth_elem()

#endif


/*
 ****************************************************************************
 * Q: Delete middle node in a singly linked list.  WIth only access to the middle node.
 *
 ****************************************************************************
 */
#if 0
Q:

Test:

Cost:


void
del_elem( list_elem_t *p_curr )
{
    List_elem_t *p_tmp;  //required to free the next node memory

    //sanity
    If (NULL == p_curr) || (NULL == p_curr->next) {
        Goto done;
}

p_tmp = p_curr->next;

p_curr->data = p_tmp->data;
p_curr->next = p_tmp->next;

free(p_tmp);
done:
    return;
} //del_elem()

#endif



#if 1
/*
Q: Write code to partion a linked list around a value X, such that all nodes less than X come before all nodes greaert then or equal to X.

Questions:
    Is X the node or the value of the Node? Value of Node
    Are there any memory constraints? RAM
    What is the side of the list? unbound
    What are the return values? A list PTR
    To clarify, partitioned means not sorted, just guaranteed as ( < X < )?  Yes
Tests:
    Empty list
    Single element list
    Assume list is not corrupted
    all < x
    all > x
    mixed case
Proposal:
    Walk list
    Partition into L1 and L2
    L1 -> L2
    Return L1
Complexity:
    T=O(n), S=O(1)
*/



list_elem_t *
list_partition( list_elem_t *p_elem,
                uint64_t     value )
{
    list_elem_t *p_lowtail  = NULL;
    list_elem_t *p_hightail = NULL;
    list_elem_t *p_head     = NULL;

    list_elem_t *p_lowhead  = p_lowtail;
    list_elem_t *p_highhead = p_hightail;

    if ((NULL == p_elem) || (NULL == p_elem->next)) {
        return p_elem;
    }

    while (p_elem) {
        // remove and isolate element from list to classify
        list_elem_t *p_tmp = p_elem;
        p_elem = p_elem->next;
        p_tmp->next = NULL;

        if (p_tmp->value >= value) {
            //add to high list
            if (NULL == p_hightail) {
                p_hightail = p_tmp;
            }else {
                p_hightail->next = p_tmp;
                p_hightail = p_hightail->next;
            }
        }else {
            //add to low list
            if (NULL == p_lowtail) {
                p_lowtail = p_tmp;
            }else {
                p_lowtail->next = p_tmp;
                p_lowtail = p_lowtail->next;
            }
        }
    }

    //merge lists
    if (NULL == p_lowhead) {
        p_head = p_highhead;
    }else {
        p_lowtail->next = p_highhead;
        p_head = p_lowtail;
    }

    // return head of low list
    return p_head;
} /* list_partition() */

#endif


/******************************************************************************
   #     # #     #   ###   ####### ####### #######  #####  #######  #####
   #     # ##    #    #       #       #    #       #     #    #    #     #
   #     # # #   #    #       #       #    #       #          #    #
   #     # #  #  #    #       #       #    #####    #####     #     #####
   #     # #   # #    #       #       #    #             #    #          #
   #     # #    ##    #       #       #    #       #     #    #    #     #
    #####  #     #   ###      #       #    #######  #####     #     #####
******************************************************************************/

static void
utest_control( void )
{


    CDISPLAY("=========================================================");

    {

    }

    return;
} /* utest_control() */

/*
 ****************************************************************************
 * test entrypoint
 *
 ****************************************************************************
 */
void
utest_cci_c2( void )
{
    CDISPLAY("=========================================================");

    utest_control();

    return;
} /* utest_cci_c2() */
