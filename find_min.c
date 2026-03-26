/**
 * @file    find_min.c
 * @brief   Finds the node with the minimum key in a Red-Black Tree (sub)tree.
 *
 * In any Binary Search Tree, the minimum key is always the leftmost reachable
 * node — the node you reach by following left child pointers until there are
 * no more left children. This is a direct consequence of the BST ordering
 * property: every left child key is strictly less than its parent's key.
 *
 * ─── ALGORITHM TRACE ──────────────────────────────────────────────────────
 *
 *      16(B)
 *     /     \
 *   7(B)   25(R)
 *   /
 *  2(B)
 *  /
 * 1(R)
 *
 *  find_min(root=16):
 *    16->left exists (7) → recurse left
 *  find_min(root=7):
 *    7->left exists (2) → recurse left
 *  find_min(root=2):
 *    2->left exists (1) → recurse left
 *  find_min(root=1):
 *    1->left == NULL → return node(1)   ← minimum
 *
 * ─── USAGE IN THIS PROJECT ────────────────────────────────────────────────
 *
 *  1. main.c   — Case 5 (Find MIN) and Case 7 (Delete MIN) in the menu.
 *  2. delete.c — Used inside delete_t() to find the IN-ORDER SUCCESSOR
 *                of a node with two children. When deleting such a node,
 *                the successor is find_min(z->right): the minimum of the
 *                right subtree. The successor physically replaces z.
 *
 * @see find_max.c  for the symmetric maximum operation
 * @see delete.c    for the in-order successor use case
 * @see tree.h      for node_t definition and prototype
 */

#include "tree.h"


/**
 * @brief  Returns the node with the smallest key in the (sub)tree.
 *
 * Recursively follows left child pointers until the leftmost node is reached.
 * The leftmost node has no left child — it is the minimum.
 *
 * @param root  The root of the (sub)tree to search.
 *              Pass the tree root for the global minimum.
 *              Pass a subtree root (e.g., z->right) for a local minimum.
 *              Passing NULL is safe — returns NULL immediately.
 * @return      Pointer to the minimum node.
 *              NULL if root is NULL (empty tree or empty subtree).
 */
node_t *find_min(node_t *root)
{
    /*
     * Guard against an empty tree or an empty subtree.
     * Without this check, the following line would dereference NULL.
     */
    if (root == NULL)
        return NULL;

    /*
     * Base case: no left child means this IS the minimum.
     * There is no smaller key reachable from this node.
     */
    if (root->left == NULL)
        return root;

    /*
     * Recursive case: a left child exists, so the minimum must be
     * somewhere further to the left.
     */
    else
        return find_min(root->left);
}