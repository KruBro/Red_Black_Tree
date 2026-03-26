/**
 * @file    search.c
 * @brief   Recursive BST key lookup for the Red-Black Tree.
 *
 * Search is the simplest read-only operation on the tree. It exploits the
 * BST ordering property (left subtree keys < node key < right subtree keys)
 * to eliminate half the remaining tree at every step, achieving O(log n)
 * time complexity guaranteed by the Red-Black height bound.
 *
 * Red-Black coloring is ENTIRELY IRRELEVANT for search. The algorithm only
 * cares about the key stored at each node and which direction to descend.
 * This is intentional — the RB properties exist to bound the tree height,
 * and search benefits from that bound implicitly.
 *
 * ─── ALGORITHM TRACE (searching for key=25 in a sample tree) ─────────────
 *
 *         16(B)
 *        /     \
 *      7(B)   25(R)
 *     /   \   /   \
 *   2(B) 15(R) 18(B) 30(B)
 *
 *   search_t(root=16, key=25):
 *     25 > 16 → go right
 *   search_t(root=25, key=25):
 *     25 == 25 → found! return this node.
 *
 * @see tree.h     for node_t definition and prototype
 * @see delete.c   for the primary caller of search_t (to find the deletion target)
 */

#include "tree.h"


/**
 * @brief  Recursively searches for a key in the Red-Black Tree.
 *
 * Base cases:
 *   - root == NULL: The subtree is empty (or we've reached a NIL leaf).
 *                   The key does not exist in this path. Return NULL.
 *   - root->data == key: Found. Return a pointer to this node.
 *
 * Recursive cases:
 *   - key > root->data: The target must be in the RIGHT subtree (larger keys).
 *   - key < root->data: The target must be in the LEFT subtree (smaller keys).
 *
 * @param root  Starting node. Pass the tree root for a global search;
 *              pass a subtree root to search within that subtree.
 *              Passing NULL is safe — returns NULL immediately (empty tree).
 * @param key   The integer key to search for.
 * @return      Pointer to the node containing key, or NULL if not found.
 */
node_t *search_t(node_t *root, int key)
{
    /*
     * Base case: NULL means either we started with an empty tree, or we have
     * recursed past a leaf into a NIL sentinel. In either case, the key
     * does not exist in this subtree.
     */
    if (root == NULL || root->data == key)
        return root;

    /* Key is larger → it must be in the right subtree (if it exists at all). */
    if (key > root->data)
        return search_t(root->right, key);

    /* Key is smaller → it must be in the left subtree. */
    else
        return search_t(root->left, key);
}