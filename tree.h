/**
 * @file    tree.h
 * @brief   Central header for the Red-Black Tree implementation.
 *
 * This file is the single source of truth for the project's type system and
 * public API. Every .c file in the project includes only this header.
 *
 * ─── RED-BLACK TREE INVARIANTS ────────────────────────────────────────────────
 *
 *  The following four properties MUST hold after every insert and delete
 *  operation. All balancing code in insertion.c, delete.c, and rotation.c
 *  exists solely to restore these properties when they are violated.
 *
 *  Property 1 — Node Color:
 *      Every node is colored either RED or BLACK. There is no third state.
 *
 *  Property 2 — Root is BLACK:
 *      The root node is always BLACK. After rotations or recoloring, the root
 *      must be explicitly set back to BLACK (see the final line of balance_t).
 *
 *  Property 3 — No Consecutive RED Nodes (Red-Red violation):
 *      A RED node may not have a RED parent. Equivalently, every RED node must
 *      have two BLACK children. This property is what insert balancing repairs.
 *
 *  Property 4 — Uniform Black-Height:
 *      Every path from any given node down to a NIL leaf must contain exactly
 *      the same number of BLACK nodes. This is the black-height of that node.
 *      Deletion repair (delete_fix_up) exists to restore this property.
 *
 *  NIL Leaves Convention:
 *      NULL pointers ARE the NIL sentinel leaves. They are always treated as
 *      BLACK (Property 1 + 4). The get_color() wrapper in tree_utils.c enforces
 *      this: calling get_color(NULL) safely returns BLACK without a segfault.
 *
 * ─── TIME COMPLEXITY GUARANTEE ────────────────────────────────────────────────
 *
 *  Because Properties 1–4 bound the tree height to at most 2*log₂(n+1),
 *  all operations (insert, delete, search, find_min, find_max) run in
 *  O(log n) worst-case time.
 *
 * ─── DEPENDENCIES ─────────────────────────────────────────────────────────────
 *  <stdio.h>   — printf, scanf (used in main.c and display.c)
 *  <stdlib.h>  — malloc, free (used in insertion.c, delete.c, tree_utils.c)
 */

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 1 — ENUMERATIONS
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @enum  color_t
 * @brief The two legal colors for a Red-Black Tree node.
 *
 * The explicit integer values (RED=0, BLACK=1) match the display format
 * used in display_tree(): "(data)--(RED->0)" and "(data)--(BLACK->1)".
 * Do not change these values without updating display.c.
 */
typedef enum {
    RED   = 0,
    BLACK = 1
} color_t;

/**
 * @enum  Status
 * @brief Return codes for operations that can succeed or fail meaningfully.
 *
 * Currently used by insert() to distinguish a duplicate-key rejection
 * from a malloc failure. Future operations should use this enum rather
 * than raw int return values.
 *
 * DATA_NOT_FOUND and TREE_EMPTY are defined for future use; they are not
 * yet returned by any function in the current implementation.
 */
typedef enum {
    SUCCESS       = 0,  /**< Operation completed successfully.                */
    FAILURE       = 1,  /**< General failure (malloc error, duplicate key).   */
    DATA_NOT_FOUND = 2, /**< Reserved: key was not found in the tree.         */
    TREE_EMPTY    = 3   /**< Reserved: operation attempted on an empty tree.  */
} Status;


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 2 — DATA STRUCTURES
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @struct node
 * @brief  A single node in the Red-Black Tree.
 *
 * Memory layout:
 *   - data   : the stored integer value (the BST key).
 *   - color  : RED or BLACK — governs balancing decisions.
 *   - left   : pointer to the left child (smaller keys), or NULL (NIL leaf).
 *   - right  : pointer to the right child (larger keys), or NULL (NIL leaf).
 *   - parent : pointer to the parent node, or NULL if this node is the root.
 *
 * The parent pointer is essential for Red-Black Tree operations. Standard BST
 * implementations often omit it, but rotations and fix-up routines require the
 * ability to walk UP the tree from a newly inserted or transplanted node.
 *
 * Diagram of a single node with two NULL (NIL) children:
 *
 *          ┌─────────────┐
 *          │  data = 10  │
 *          │  color = RED│
 *          │  parent → ? │
 *          └──────┬──────┘
 *            ┌───┴────┐
 *           NULL     NULL
 *          (NIL,B)  (NIL,B)
 */
typedef struct node {
    int         data;   /**< Integer key stored at this node.                 */
    color_t     color;  /**< Color of this node: RED or BLACK.                */
    struct node *left;  /**< Left child (key < this->data), or NULL.          */
    struct node *right; /**< Right child (key > this->data), or NULL.         */
    struct node *parent;/**< Parent node, or NULL if this is the root.        */
} node_t;


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 3 — FUNCTION PROTOTYPES
 * Grouped by the .c file that implements them.
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── display.c ────────────────────────────────────────────────────────────── */

/**
 * @brief  Prints the tree contents via an in-order (LNR) traversal.
 *
 * In-order traversal of a BST visits nodes in ascending key order, so the
 * output is a sorted sequence of all stored values. Each node is printed as:
 *     (data)--(RED->0)    or    (data)--(BLACK->1)
 * with ANSI terminal colors applied (requires color.h).
 *
 * @param root  Pointer to the root of the (sub)tree to display.
 *              Passing NULL is safe — it produces no output (base case).
 */
void display_tree(node_t *root);


/* ── insertion.c ──────────────────────────────────────────────────────────── */

/**
 * @brief  Inserts a new key into the Red-Black Tree.
 *
 * Performs a standard iterative BST insertion followed by a call to
 * balance_t() to restore any Red-Black properties violated by the insertion.
 * Duplicate keys are rejected silently (returns FAILURE, no tree mutation).
 *
 * @param root  Double pointer to the root. May be updated if the new node
 *              becomes the root or if rotations elevate a different node.
 * @param data  The integer key to insert.
 * @return      SUCCESS if the node was inserted and the tree is balanced.
 *              FAILURE if data is a duplicate, or if malloc() returns NULL.
 */
Status insert(node_t **root, int data);

/**
 * @brief  Restores Red-Black properties after an insertion (Property 3).
 *
 * Called recursively from the newly inserted node upward toward the root.
 * Resolves Red-Red violations by examining the color of the "uncle" node
 * (the sibling of the violating node's parent):
 *
 *   Uncle is RED  →  Recolor parent, uncle, and grandparent; recurse upward.
 *   Uncle is BLACK →  Perform the appropriate rotation (LL/RR/LR/RL) via rotate().
 *
 * This function must guarantee that (*root)->color == BLACK when it returns.
 *
 * @param root      Double pointer to the tree root (may change after rotation).
 * @param new_node  The node that was just inserted (or the grandparent being
 *                  re-examined after a recolor step).
 */
void balance_t(node_t **root, node_t *new_node);


/* ── search.c ─────────────────────────────────────────────────────────────── */

/**
 * @brief  Searches for a key in the Red-Black Tree (recursive BST lookup).
 *
 * Standard BST search: go left if key < current node, right if key > current.
 * Red-Black coloring is irrelevant for search — the BST ordering property
 * alone determines the path.
 *
 * @param root  The node to start searching from (pass the tree root initially).
 * @param key   The integer key to find.
 * @return      Pointer to the node containing key, or NULL if not found.
 *              Also returns NULL if root is NULL (empty tree / NIL leaf).
 */
node_t *search_t(node_t *root, int key);


/* ── find_min.c ───────────────────────────────────────────────────────────── */

/**
 * @brief  Returns the node with the smallest key in the (sub)tree.
 *
 * In a BST, the minimum is always the leftmost reachable node. This function
 * follows left child pointers until it reaches a node with no left child.
 *
 * Used internally by delete_t() to find the in-order successor (the minimum
 * of the right subtree) when deleting a node with two children.
 *
 * @param root  Starting node (pass the tree root for the global minimum,
 *              or a subtree root for a local minimum).
 * @return      Pointer to the minimum node, or NULL if root is NULL.
 */
node_t *find_min(node_t *root);


/* ── find_max.c ───────────────────────────────────────────────────────────── */

/**
 * @brief  Returns the node with the largest key in the (sub)tree.
 *
 * Symmetrically to find_min(), follows right child pointers until it reaches
 * a node with no right child.
 *
 * @param root  Starting node.
 * @return      Pointer to the maximum node, or NULL if root is NULL.
 */
node_t *find_max(node_t *root);


/* ── tree_utils.c ─────────────────────────────────────────────────────────── */

/**
 * @brief  Toggles a node's color between RED and BLACK.
 *
 * Used during recoloring in balance_t(). Returns the opposite color — it does
 * NOT modify the node in place. The caller is responsible for assignment.
 *
 * Example:  uncle->color = recolor(uncle);
 *
 * @param node  The node whose color to flip. Must NOT be NULL.
 * @return      BLACK if node->color is RED; RED if node->color is BLACK.
 */
color_t recolor(node_t *node);

/**
 * @brief  NULL-safe color reader. Returns the color of a node or BLACK for NULL.
 *
 * This is the canonical way to read a node's color anywhere in the codebase.
 * Calling node->color directly on a NULL pointer (a NIL leaf) causes a
 * segmentation fault. By convention, NIL leaves are BLACK (RB Property 4),
 * so get_color(NULL) correctly returns BLACK.
 *
 * Every color check in delete_fix_up() uses this wrapper.
 *
 * @param node  Any node_t pointer, including NULL.
 * @return      node->color if node is non-NULL; BLACK if node is NULL.
 */
color_t get_color(node_t *node);

/**
 * @brief  NULL-safe color writer. Sets the color of a node if it is non-NULL.
 *
 * Pairs with get_color() for all safe color mutation in delete.c.
 * Writing to a NULL pointer (a NIL sentinel) is a no-op — NIL leaves are
 * always BLACK and do not need to be explicitly colored.
 *
 * @param node   The node to recolor. If NULL, this function does nothing.
 * @param color  The color to assign: RED or BLACK.
 */
void set_color(node_t *node, color_t color);

/**
 * @brief  Recursively frees all nodes in the tree (post-order traversal).
 *
 * Uses a post-order (LRN) traversal so that children are always freed before
 * their parent — freeing a parent first would lose the child pointers.
 * Passing NULL is safe (base case, returns immediately).
 *
 * Called by Case 9 (Exit) in main.c to prevent memory leaks at shutdown.
 * Valgrind confirms 0 bytes leaked after this call on the full test suite.
 *
 * @param root  Root of the (sub)tree to free. May be NULL.
 */
void free_tree(node_t *root);


/* ── delete.c ─────────────────────────────────────────────────────────────── */

/**
 * @brief  Transplants subtree rooted at 'v' into the position of subtree 'u'.
 *
 * A low-level helper that rewires parent pointers to replace node u with node v
 * in the tree structure. After this call, u's parent points to v instead of u.
 * The nodes u and v themselves are NOT freed or modified beyond their parent links.
 *
 * Note: v may be NULL (representing a NIL leaf replacement). In that case,
 * the parent link on v is skipped to avoid a NULL dereference.
 *
 * @param root  Double pointer to the tree root (updated if u was the root).
 * @param u     The node being replaced. Must NOT be NULL.
 * @param v     The node taking u's position. May be NULL.
 */
void transplant(node_t **root, node_t *u, node_t *v);

/**
 * @brief  Deletes the node with the given key from the Red-Black Tree.
 *
 * Implements the full CLRS Red-Black deletion algorithm:
 *   1. Locate the target node z via search_t().
 *   2. Determine y (the node physically removed from the tree) and x
 *      (the node that moves into y's former position).
 *   3. Perform the BST transplant, handling three structural cases:
 *        a. z has no left child  → replace z with its right child.
 *        b. z has no right child → replace z with its left child.
 *        c. z has two children   → replace z with its in-order successor y
 *                                   (minimum of z's right subtree), then
 *                                   move y into z's position.
 *   4. If y's original color was BLACK, call delete_fix_up() to repair
 *      the black-height violation introduced by removing a BLACK node.
 *
 * The global variable `status` (declared in main.c) is set to 1 if the
 * key was found and deleted, or 0 if the key was not found.
 *
 * @param root  The current root of the tree.
 * @param key   The integer key to delete.
 * @return      The new root of the tree (may change if the root was deleted
 *              or if fix-up rotations elevated a different node).
 */
node_t *delete_t(node_t *root, int key);

/**
 * @brief  Restores Red-Black properties after deletion of a BLACK node.
 *
 * Deletion of a BLACK node reduces the black-height of every path passing
 * through it by 1, creating a "Double Black" violation of Property 4.
 * This routine resolves the violation by iterating up the tree, classifying
 * the situation into one of 4 cases (plus 4 mirror-image cases for right
 * children) based on the sibling and nephew node colors:
 *
 *   Case 1 — Sibling is RED:
 *       Rotate toward x and recolor. Transforms into Case 2, 3, or 4.
 *
 *   Case 2 — Sibling is BLACK, both nephews are BLACK:
 *       Recolor sibling RED. Push the Double Black problem up to the parent.
 *       The loop continues at x_parent.
 *
 *   Case 3 — Sibling is BLACK, far nephew is BLACK, near nephew is RED:
 *       Rotate away from x at sibling, recolor. Transforms into Case 4.
 *
 *   Case 4 — Sibling is BLACK, far nephew is RED:
 *       Rotate toward x at x_parent, recolor. Tree is balanced; loop exits.
 *
 * The x_parent parameter is tracked separately because x itself is frequently
 * NULL (a NIL leaf replacement), and NULL->parent is undefined behavior.
 *
 * @param root      Double pointer to the tree root (may change after rotations).
 * @param x         The node that has absorbed the "double black" deficit.
 *                  May be NULL.
 * @param x_parent  The parent of x. Tracked separately because x may be NULL.
 */
void delete_fix_up(node_t **root, node_t *x, node_t *x_parent);


/* ── rotation.c ───────────────────────────────────────────────────────────── */

/**
 * @brief  Performs a left rotation around node x.
 *
 * A left rotation pivots x down-left and elevates x->right (call it y)
 * into x's former position. y's left subtree becomes x's right subtree.
 *
 * Before:          After:
 *      x                y
 *     / \              / \
 *    A   y     →      x   C
 *       / \          / \
 *      B   C        A   B
 *
 * Preserves the BST ordering invariant: A < x < B < y < C.
 * Used in: RR insert case, Case 1 and Case 4 of delete_fix_up (left path).
 *
 * @param root  Double pointer to the tree root (updated if x was the root).
 * @param x     The node to rotate around. Must not be NULL, and x->right
 *              must not be NULL (checked internally; no-op if violated).
 */
void left_rotation(node_t **root, node_t *x);

/**
 * @brief  Performs a right rotation around node x.
 *
 * Mirror image of left_rotation. Pivots x down-right and elevates x->left
 * (call it y) into x's former position. y's right subtree becomes x's left.
 *
 * Before:          After:
 *      x                y
 *     / \              / \
 *    y   C     →      A   x
 *   / \                  / \
 *  A   B                B   C
 *
 * Used in: LL insert case, Case 1 and Case 4 of delete_fix_up (right path).
 *
 * @param root  Double pointer to the tree root (updated if x was the root).
 * @param x     The node to rotate around. Must not be NULL, and x->left
 *              must not be NULL (checked internally; no-op if violated).
 */
void right_rotation(node_t **root, node_t *x);

/**
 * @brief  Routes insert imbalances to the correct rotation sequence.
 *
 * Called by balance_t() when the uncle is BLACK. Determines the structural
 * relationship between the new node (n), its parent (p), and its grandparent
 * (g), then dispatches to the appropriate rotation(s) and recoloring:
 *
 *   LL (n is p->left,  p is g->left)  → right_rotation(g); swap colors g,p.
 *   RR (n is p->right, p is g->right) → left_rotation(g);  swap colors g,p.
 *   LR (n is p->right, p is g->left)  → left_rotation(p) then right_rotation(g);
 *                                        swap colors n,g.
 *   RL (n is p->left,  p is g->right) → right_rotation(p) then left_rotation(g);
 *                                        swap colors n,g.
 *
 * @param root  Double pointer to the tree root.
 * @param n     The newly inserted (or currently imbalanced) node.
 *              n->parent and n->parent->parent must both be non-NULL.
 */
void rotate(node_t **root, node_t *n);


#endif /* TREE_H */