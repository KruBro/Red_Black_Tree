/**
 * @file    find_max.c
 * @brief   Finds the node with the maximum key in a Red-Black Tree (sub)tree.
 *
 * Symmetric counterpart to find_min.c. In any Binary Search Tree, the maximum
 * key is always the rightmost reachable node — found by following right child
 * pointers until there are no more right children. Every right child key is
 * strictly greater than its parent's key by the BST ordering property.
 *
 * ─── ALGORITHM TRACE ──────────────────────────────────────────────────────
 *
 *      16(B)
 *     /     \
 *   7(B)   25(R)
 *              \
 *              40(B)
 *                \
 *                70(R)
 *
 *  find_max(root=16):
 *    16->right exists (25) → recurse right
 *  find_max(root=25):
 *    25->right exists (40) → recurse right
 *  find_max(root=40):
 *    40->right exists (70) → recurse right
 *  find_max(root=70):
 *    70->right == NULL → return node(70)   ← maximum
 *
 * ─── USAGE IN THIS PROJECT ────────────────────────────────────────────────
 *
 *  main.c — Case 6 (Find MAX) and Case 8 (Delete MAX) in the interactive menu.
 *           Unlike find_min(), find_max() is NOT called internally by any
 *           deletion algorithm — it is purely a user-facing utility.
 *
 * @see find_min.c  for the symmetric minimum operation
 * @see tree.h      for node_t definition and prototype
 */

#include "tree.h"


/**
 * @brief  Returns the node with the largest key in the (sub)tree.
 *
 * Recursively follows right child pointers until the rightmost node is reached.
 * The rightmost node has no right child — it is the maximum.
 *
 * @param root  The root of the (sub)tree to search.
 *              Pass the tree root for the global maximum.
 *              Passing NULL is safe — returns NULL immediately.
 * @return      Pointer to the maximum node.
 *              NULL if root is NULL (empty tree).
 */
node_t *find_max(node_t *root)
{
    /*
     * Guard against an empty tree.
     * Without this check, the following access would dereference NULL.
     */
    if (root == NULL)
        return NULL;

    /*
     * Base case: no right child means this IS the maximum.
     * There is no larger key reachable from this node.
     */
    if (root->right == NULL)
        return root;

    /*
     * Recursive case: a right child exists, so the maximum must be
     * somewhere further to the right.
     */
    else
        return find_max(root->right);
}