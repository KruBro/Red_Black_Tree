/**
 * @file    rotation.c
 * @brief   Pure mechanical pointer-swap rotations for Red-Black Tree rebalancing.
 *
 * Contains three functions:
 *
 *   left_rotation()  — Pivots a node downward-left, elevating its right child.
 *   right_rotation() — Pivots a node downward-right, elevating its left child.
 *   rotate()         — Router: inspects the LL/RR/LR/RL imbalance shape and
 *                      dispatches to the correct rotation sequence with recoloring.
 *
 * ─── WHAT ROTATIONS DO ────────────────────────────────────────────────────
 *
 *  Rotations are the fundamental restructuring primitive of self-balancing trees.
 *  They change the shape of the tree without changing the in-order traversal
 *  (BST ordering is strictly preserved). They are O(1) — exactly a fixed
 *  number of pointer assignments regardless of tree size.
 *
 *  Critically, rotations are COLORBLIND: left_rotation and right_rotation
 *  perform no recoloring. Only rotate() (the router) handles color swaps,
 *  because the correct recoloring depends on the structural shape (LL/RR/LR/RL)
 *  rather than on the mechanical direction of the rotation.
 *
 * ─── ROTATION VISUAL REFERENCE ────────────────────────────────────────────
 *
 *  Left Rotation around X:           Right Rotation around X:
 *
 *      X                  Y              X                  Y
 *     / \      →         / \            / \      →         / \
 *    A   Y              X   C          Y   C              A   X
 *       / \            / \            / \                    / \
 *      B   C          A   B          A   B                  B   C
 *
 *  In both cases, the sub-tree B "re-parents" from the elevated node (Y)
 *  to the demoted node (X). This is the only non-trivial pointer link.
 *
 * ─── FOUR IMBALANCE SHAPES (for insert fix-up) ────────────────────────────
 *
 *   LL (Left-Left):   new node is left child of left child of grandparent.
 *                     Fix: single right rotation on grandparent.
 *
 *   RR (Right-Right): new node is right child of right child of grandparent.
 *                     Fix: single left rotation on grandparent.
 *
 *   LR (Left-Right):  new node is right child of left child of grandparent.
 *                     Fix: left rotation on parent, then right rotation on grandparent.
 *
 *   RL (Right-Left):  new node is left child of right child of grandparent.
 *                     Fix: right rotation on parent, then left rotation on grandparent.
 *
 * @see insertion.c  for balance_t(), which calls rotate()
 * @see delete.c     for delete_fix_up(), which calls left_rotation/right_rotation directly
 * @see tree.h       for node_t definition
 */

#include "tree.h"


/**
 * @brief  Performs a left rotation around node x.
 *
 * Elevates x->right (called y) into x's position. x becomes y's left child.
 * y's original left subtree (B in the diagram below) becomes x's right subtree,
 * which is the only sub-tree that changes parents during a left rotation.
 *
 *   Before:                After:
 *        x                    y
 *       / \                  / \
 *      A   y       →        x   C
 *         / \              / \
 *        B   C            A   B
 *
 *   In-order preserved: A < x < B < y < C
 *
 * Pointer operations (in order):
 *   1. y's left subtree (B) moves to x->right.
 *   2. If B is non-NULL, B's parent is updated to x.
 *   3. y's parent is updated to x's former parent.
 *   4. If x was the root, y becomes the new root.
 *      Otherwise, update x's parent to point to y instead of x.
 *   5. y->left = x; x->parent = y.  (x descends under y)
 *
 * @param root  Double pointer to the tree root. Updated if x was the root.
 * @param x     Node to rotate around. No-op if x is NULL or x->right is NULL.
 */
void left_rotation(node_t **root, node_t *x)
{
    /* Safety: can't rotate if x doesn't exist or has no right child to elevate. */
    if (x == NULL || x->right == NULL)
        return;

    node_t *y = x->right;  /* y is the node that will rise into x's position. */

    /* Step 1 & 2: y's left subtree (B) is rehomed as x's right subtree. */
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;   /* B's parent now points back to x, not y. */

    /* Step 3 & 4: Wire y up to x's former parent. */
    y->parent = x->parent;

    if (x->parent == NULL) {
        *root = y;                      /* x was root → y is new root.     */
    } else if (x == x->parent->left) {
        x->parent->left = y;            /* x was a left child → y replaces it. */
    } else {
        x->parent->right = y;           /* x was a right child → y replaces it. */
    }

    /* Step 5: x becomes y's left child. */
    y->left = x;
    x->parent = y;
}


/**
 * @brief  Performs a right rotation around node x.
 *
 * Mirror image of left_rotation. Elevates x->left (called y) into x's position.
 * x becomes y's right child. y's original right subtree (B) becomes x's left subtree.
 *
 *   Before:                After:
 *        x                    y
 *       / \                  / \
 *      y   C       →        A   x
 *     / \                      / \
 *    A   B                    B   C
 *
 *   In-order preserved: A < y < B < x < C
 *
 * Pointer operations (in order):
 *   1. y's right subtree (B) moves to x->left.
 *   2. If B is non-NULL, B's parent is updated to x.
 *   3. y's parent is updated to x's former parent.
 *   4. Update root or x's parent to point to y.
 *   5. y->right = x; x->parent = y.
 *
 * @param root  Double pointer to the tree root. Updated if x was the root.
 * @param x     Node to rotate around. No-op if x is NULL or x->left is NULL.
 */
void right_rotation(node_t **root, node_t *x)
{
    /* Safety: can't rotate if x doesn't exist or has no left child to elevate. */
    if (x == NULL || x->left == NULL)
        return;

    node_t *y = x->left;  /* y is the node that will rise into x's position. */

    /* Step 1 & 2: y's right subtree (B) is rehomed as x's left subtree. */
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;   /* B's parent now points back to x, not y. */

    /* Step 3 & 4: Wire y up to x's former parent. */
    y->parent = x->parent;

    if (x->parent == NULL) {
        *root = y;                      /* x was root → y is new root.     */
    } else if (x == x->parent->left) {
        x->parent->left = y;            /* x was a left child → y replaces it. */
    } else {
        x->parent->right = y;           /* x was a right child → y replaces it. */
    }

    /* Step 5: x becomes y's right child. */
    y->right = x;
    x->parent = y;
}


/**
 * @brief  Determines the imbalance shape and dispatches to the correct rotation.
 *
 * Called by balance_t() after it establishes that a Red-Red violation exists
 * and the uncle is BLACK. At this point a rotation is required.
 *
 * Let:  n = new (violating) node, p = n's parent, g = p's parent (grandparent).
 *
 * ─── CASE LL ──────────────────────────────────────────────────────────────
 *   n is left child of p, p is left child of g.
 *   Structure:       g(B)             Fix: right_rotation(g)
 *                   /                 Recolor: g → RED, p → BLACK
 *                  p(R)               (p rises, g demotes — swap their colors)
 *                 /
 *                n(R)
 *
 * ─── CASE RR ──────────────────────────────────────────────────────────────
 *   n is right child of p, p is right child of g.
 *   Structure:   g(B)                 Fix: left_rotation(g)
 *                 \                   Recolor: g → RED, p → BLACK
 *                  p(R)
 *                   \
 *                    n(R)
 *
 * ─── CASE LR ──────────────────────────────────────────────────────────────
 *   n is right child of p, p is left child of g.
 *   Structure:    g(B)                Fix: left_rotation(p) → converts to LL,
 *               /                          then right_rotation(g)
 *              p(R)                   Recolor: g → RED, n → BLACK
 *               \                     (n rises to grandparent position)
 *                n(R)
 *
 * ─── CASE RL ──────────────────────────────────────────────────────────────
 *   n is left child of p, p is right child of g.
 *   Structure:   g(B)                 Fix: right_rotation(p) → converts to RR,
 *                  \                        then left_rotation(g)
 *                   p(R)              Recolor: g → RED, n → BLACK
 *                  /
 *                n(R)
 *
 * @param root  Double pointer to the tree root (may change after rotation).
 * @param n     The newly inserted node causing the Red-Red violation.
 *              n->parent and n->parent->parent must both be non-NULL.
 *              (Callers guarantee this — balance_t() checks before calling.)
 */
void rotate(node_t **root, node_t *n)
{
    node_t *p = n->parent;
    if (p == NULL || p->parent == NULL)
        return;  /* Safety check: rotation requires both parent and grandparent. */

    node_t *g = p->parent;

    /* ── LL Case: single right rotation on grandparent ─────────────────── */
    if (n == p->left && p == g->left) {
        right_rotation(root, g);
        /*
         * After the rotation, p has risen to g's former position.
         * g has become p's right child. Swap their colors:
         *   g was BLACK (it was properly colored before the violation).
         *   p was RED (caused the violation).
         * Now g → RED, p → BLACK. The subtree root (p) is BLACK, satisfying P3.
         */
        g->color = recolor(g);  /* g: BLACK → RED   */
        p->color = recolor(p);  /* p: RED   → BLACK */
    }

    /* ── RR Case: single left rotation on grandparent ──────────────────── */
    else if (n == p->right && p == g->right) {
        left_rotation(root, g);
        /*
         * Mirror of LL: p rises, g demotes. Same color swap.
         * g → RED, p → BLACK.
         */
        g->color = recolor(g);  /* g: BLACK → RED   */
        p->color = recolor(p);  /* p: RED   → BLACK */
    }

    /* ── LR Case: left rotation on parent, then right rotation on grandparent */
    else if (n == p->right && p == g->left) {
        /*
         * Step 1: Left-rotate on p. This transforms the LR shape into an LL
         *         shape with n now in p's former position and p as n's left child.
         */
        left_rotation(root, p);
        /*
         * Step 2: Right-rotate on g. n rises to g's position.
         * Recolor: n takes BLACK (it's now the subtree root), g takes RED.
         * p keeps its existing color — it's now a leaf-level child.
         */
        right_rotation(root, g);
        n->color = recolor(n);  /* n: RED   → BLACK */
        g->color = recolor(g);  /* g: BLACK → RED   */
    }

    /* ── RL Case: right rotation on parent, then left rotation on grandparent */
    else if (n == p->left && p == g->right) {
        /*
         * Step 1: Right-rotate on p. Transforms RL into RR shape with n
         *         now in p's former position and p as n's right child.
         */
        right_rotation(root, p);
        /*
         * Step 2: Left-rotate on g. n rises to g's position.
         * Same recoloring as LR: n → BLACK, g → RED.
         */
        left_rotation(root, g);
        n->color = recolor(n);  /* n: RED   → BLACK */
        g->color = recolor(g);  /* g: BLACK → RED   */
    }
}