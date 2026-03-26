/**
 * @file    tree_utils.c
 * @brief   Utility and memory-management functions for the Red-Black Tree.
 *
 * Contains three functions that don't belong to any single algorithmic phase
 * but are shared across multiple modules:
 *
 *   recolor()    — Toggles a node's color (RED ↔ BLACK). Used in insertion.c
 *                  and rotation.c during balancing steps.
 *
 *   free_tree()  — Post-order recursive deallocation of the entire tree.
 *                  Called by main.c on exit to prevent memory leaks.
 *
 *   get_color()  — NULL-safe color reader. Defined in delete.c because it is
 *   set_color()    most heavily used there, but prototyped in tree.h for the
 *                  whole project. See delete.c for their documentation.
 *
 * ─── MEMORY MANAGEMENT DESIGN ─────────────────────────────────────────────
 *
 *  Every node_t is allocated with malloc() in insertion.c and freed with
 *  free() in exactly two places:
 *    1. delete.c → delete_t(): frees the target node z after transplanting.
 *    2. tree_utils.c → free_tree(): frees ALL nodes on program exit.
 *
 *  free_tree() uses POST-ORDER traversal (left → right → node) deliberately:
 *  freeing a node before its children would lose the pointers to those children,
 *  creating an unreachable memory leak. Post-order guarantees children are
 *  always freed before their parent.
 *
 *  Valgrind confirms 0 bytes leaked after free_tree() on the full test suite.
 *
 * @see delete.c    for get_color() and set_color() definitions
 * @see insertion.c for the primary caller of recolor()
 * @see rotation.c  for additional callers of recolor()
 * @see tree.h      for node_t, color_t, and all prototypes
 */

#include "tree.h"


/**
 * @brief  Returns the opposite color of a node's current color.
 *
 * This function is PURELY FUNCTIONAL — it does not mutate the node.
 * The caller is responsible for assigning the returned value:
 *
 *     uncle->color = recolor(uncle);        // Correct
 *     recolor(uncle);                        // Bug: no effect
 *
 * Used during insert balancing (balance_t) to simultaneously flip
 * parent, uncle, and grandparent colors in the "uncle is RED" case.
 * Also used in rotation.c to swap colors after single rotations.
 *
 * @param node  The node whose color to examine. Must NOT be NULL.
 *              (For NULL-safe color operations, use get_color/set_color.)
 * @return      BLACK if node->color == RED.
 *              RED   if node->color == BLACK.
 */
color_t recolor(node_t *node)
{
    if (node->color == RED)
        return BLACK;
    else
        return RED;
}


/**
 * @brief  Recursively frees all nodes in the tree using post-order traversal.
 *
 * Traversal order: LEFT subtree → RIGHT subtree → current node.
 * This ensures no node is freed before all of its descendants are freed,
 * which prevents losing pointer access to any allocated memory.
 *
 * ─── TRACE (freeing a 3-node tree) ───────────────────────────────────────
 *
 *       20(B)
 *      /     \
 *    10(R)  30(R)
 *
 *  free_tree(20):
 *    free_tree(10):
 *      free_tree(NULL) → return   [10's left]
 *      free_tree(NULL) → return   [10's right]
 *      free(10)
 *    free_tree(30):
 *      free_tree(NULL) → return   [30's left]
 *      free_tree(NULL) → return   [30's right]
 *      free(30)
 *    free(20)
 *
 * After this call, all heap memory used by the tree is released.
 * The caller should set the root pointer to NULL after calling free_tree()
 * to avoid a dangling pointer:
 *
 *     free_tree(root);
 *     root = NULL;    // Prevent use-after-free
 *
 * @param root  Root of the (sub)tree to free. NULL is a safe no-op (base case).
 */
void free_tree(node_t *root)
{
    /* Base case: NULL means either an empty tree or a NIL leaf. Nothing to free. */
    if (root == NULL) {
        return;
    }

    /* Post-order: free left subtree first, then right, then this node. */
    free_tree(root->left);   /* Recurse left. */
    free_tree(root->right);  /* Recurse right. */

    /*
     * Free the current node LAST.
     * At this point, root->left and root->right have both been fully freed
     * and their heap memory returned to the OS. Accessing them after this
     * free() call would be undefined behavior — but we don't need to.
     */
    free(root);
}