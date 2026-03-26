/**
 * @file    delete.c
 * @brief   Red-Black Tree deletion: BST removal, transplant, and Double Black fix-up.
 *
 * This is the most algorithmically complex file in the project. It implements
 * the full CLRS Red-Black deletion algorithm, which requires handling more cases
 * than insertion because removing a node can violate the black-height property
 * (Property 4) in ways that inserting cannot.
 *
 * ─── HIGH-LEVEL DELETION ALGORITHM ───────────────────────────────────────
 *
 *  Phase 1 — Find: locate the node z with the target key via search_t().
 *
 *  Phase 2 — Identify the "physically removed" node y and its replacement x:
 *    Case A (z has 0 or 1 child): y = z itself. x = z's only child (or NULL).
 *    Case B (z has 2 children):   y = in-order successor of z (minimum of z's
 *                                 right subtree). y has at most a right child.
 *                                 x = y's right child (may be NULL).
 *
 *  Phase 3 — BST Transplant: splice y out of the tree.
 *    Case A: transplant x directly into z's position. Free z.
 *    Case B: transplant x into y's position. Move y into z's position,
 *            copying z's color to y. Free z.
 *
 *  Phase 4 — Conditional Fix-Up:
 *    If y's ORIGINAL color (before the transplant) was BLACK, a Double Black
 *    violation may exist at x's position. Call delete_fix_up() to repair it.
 *    If y was RED, no black-height changed — no fix needed.
 *
 * ─── WHY "DOUBLE BLACK"? ──────────────────────────────────────────────────
 *
 *  Removing a BLACK node y reduces the black-count on every path that passed
 *  through y by 1. The conceptual solution is to give x an "extra" BLACK credit.
 *  x is now "doubly black" — it carries 2 units of blackness. delete_fix_up()
 *  resolves this extra credit by propagating it up the tree or absorbing it via
 *  rotations, until x reaches the root (where extra blackness is simply dropped)
 *  or encounters a RED node (which can absorb the extra credit by turning BLACK).
 *
 * ─── THE x_parent TRACKING PROBLEM ───────────────────────────────────────
 *
 *  Inside delete_fix_up(), we frequently need to access x's sibling, which
 *  requires x->parent. But x is often NULL (when the deleted node had no
 *  children, its replacement is a NIL leaf). NULL->parent is undefined behavior.
 *
 *  Solution: track x_parent separately throughout the entire deletion routine.
 *  x_parent is always kept up to date even when x itself is NULL, making it
 *  safe to compute the sibling as x_parent->left or x_parent->right.
 *
 * @see tree_utils.c  for get_color(), set_color() — used throughout this file
 * @see rotation.c    for left_rotation(), right_rotation()
 * @see search.c      for search_t() — used to find the target node
 * @see find_min.c    for find_min() — used to find the in-order successor
 * @see tree.h        for node_t, color_t, and function prototypes
 */

#include "tree.h"

extern int status; /**< Global status flag declared in main.c.
                    *   Set to 1 if deletion succeeded, 0 if key not found. */


/* ═══════════════════════════════════════════════════════════════════════════
 * UTILITY WRAPPERS (defined here, prototyped in tree.h)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief  NULL-safe color reader. Returns BLACK for NULL (NIL leaf) nodes.
 *
 * The core safety primitive of this file. Every single color check in
 * delete_fix_up() passes through this function rather than accessing
 * node->color directly. This prevents segmentation faults when examining
 * the children of a newly orphaned or leaf node, which may be NULL.
 *
 * By Red-Black convention, NIL (NULL) leaves are always BLACK.
 *
 * @param node  Any node_t pointer, including NULL.
 * @return      node->color if node != NULL; BLACK if node == NULL.
 */
color_t get_color(node_t *node)
{
    if (node == NULL) return BLACK;
    return node->color;
}

/**
 * @brief  NULL-safe color writer. No-op when node is NULL.
 *
 * Used alongside get_color() throughout delete_fix_up(). Writing a color to
 * a NIL leaf (NULL) would be meaningless — NIL leaves are always BLACK and
 * have no storage to write to. This wrapper silently ignores NULL nodes.
 *
 * @param node   The node to recolor. Ignored if NULL.
 * @param color  The color to assign (RED or BLACK).
 */
void set_color(node_t *node, color_t color)
{
    if (node != NULL) {
        node->color = color;
    }
}


/* ═══════════════════════════════════════════════════════════════════════════
 * TRANSPLANT
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief  Replaces the subtree rooted at u with the subtree rooted at v.
 *
 * This is the low-level structural primitive used by delete_t() to physically
 * remove a node from the tree. It rewires u's parent to point to v instead,
 * and sets v's parent pointer to u's former parent.
 *
 * IMPORTANT: transplant() does NOT:
 *   - Free node u (the caller must do that).
 *   - Update u's or v's left/right child pointers (the caller handles those).
 *   - Perform any rebalancing.
 *
 * After this call, u is logically disconnected from the tree.
 * v is now in u's former position.
 *
 *   Example — Transplanting v into u's position:
 *
 *      parent                     parent
 *      /    \       →             /    \
 *     u      X                  v      X
 *    / \                       / \
 *   ...  ...                  ...  ...
 *
 * @param root  Double pointer to the tree root (updated if u was the root).
 * @param u     The node being replaced. Must NOT be NULL.
 * @param v     The replacement node. May be NULL (replaces u with a NIL leaf).
 */
void transplant(node_t **root, node_t *u, node_t *v)
{
    if (u->parent == NULL) {
        /* u was the root. v becomes the new root. */
        *root = v;
    } else if (u == u->parent->left) {
        /* u was a left child — update the parent's left pointer. */
        u->parent->left = v;
    } else {
        /* u was a right child — update the parent's right pointer. */
        u->parent->right = v;
    }

    /*
     * Set v's parent back to u's former parent.
     * Skipped if v is NULL — NIL leaves have no parent field to update.
     */
    if (v != NULL) {
        v->parent = u->parent;
    }
}


/* ═══════════════════════════════════════════════════════════════════════════
 * DELETE FIX-UP
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief  Restores Property 4 (uniform black-height) after a BLACK node is removed.
 *
 * x is the node that has absorbed the "double black" deficit — it carries
 * one extra unit of blackness that must be resolved before the tree is valid.
 *
 * The loop continues as long as x is not the root AND x's color is BLACK
 * (if x is RED, we can simply paint it BLACK to absorb the extra credit — done
 * by the final set_color(x, BLACK) after the loop).
 *
 * ─── FOUR CASES (x is a LEFT child) ──────────────────────────────────────
 *
 *  Let w = the sibling of x (x_parent's other child).
 *
 *  Case 1: w is RED.
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │    xp(B)              w(B)                                          │
 *    │   /     \    →       /   \                                          │
 *    │  x(DB)  w(R)       xp(R)  D(B)                                     │
 *    │         / \        /   \                                            │
 *    │       C(B) D(B)  x(DB)  C(B)   ← w's left child C is new sibling  │
 *    │                                   Continue to Case 2, 3, or 4.     │
 *    │  Action: set_color(w,BLACK), set_color(xp,RED), left_rotation(xp)  │
 *    │          w = xp->right (the new sibling after rotation)             │
 *    └─────────────────────────────────────────────────────────────────────┘
 *
 *  Case 2: w is BLACK, w->left is BLACK, w->right is BLACK.
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │    xp(?)              xp(DB?)                                       │
 *    │   /     \    →       /      \                                        │
 *    │  x(DB)  w(B)       x(B)    w(R)   ← w gives one BLACK up to xp     │
 *    │         / \                / \                                       │
 *    │       C(B) D(B)          C(B) D(B)  xp is now "doubly black"        │
 *    │                                      Loop continues upward.          │
 *    │  Action: set_color(w,RED), x = xp, xp = x->parent                   │
 *    └─────────────────────────────────────────────────────────────────────┘
 *
 *  Case 3: w is BLACK, w->right is BLACK, w->left is RED.
 *    Transforms into Case 4 by rotating at w.
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │         xp                    xp                                    │
 *    │        /  \      →           /  \                                   │
 *    │      x(DB) w(B)           x(DB)  C(B)  ← C rises, new w            │
 *    │            / \                     \                                │
 *    │          C(R) D(B)                  w(R)                            │
 *    │                                      \                              │
 *    │  Action: set_color(w->left,BLACK),    D(B)    (now Case 4)          │
 *    │          set_color(w,RED), right_rotation(w), w = xp->right         │
 *    └─────────────────────────────────────────────────────────────────────┘
 *
 *  Case 4: w is BLACK, w->right is RED.
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │     xp(?)             w(?)                                          │
 *    │    /     \    →      /    \                                          │
 *    │  x(DB)   w(B)      xp(B)  D(B)   ← xp absorbs x's extra black      │
 *    │          / \       /   \                                             │
 *    │        C(?) D(R)  x(B)  C(?)   Tree is now valid. Loop exits.       │
 *    │                                                                      │
 *    │  Action: set_color(w, color(xp)), set_color(xp,BLACK),              │
 *    │          set_color(w->right,BLACK), left_rotation(xp), x = *root   │
 *    └─────────────────────────────────────────────────────────────────────┘
 *
 *  Cases 1–4 are mirrored for when x is a RIGHT child (w is the left sibling).
 *
 * @param root      Double pointer to the tree root.
 * @param x         The "double black" node. Frequently NULL — must never be
 *                  dereferenced directly; use get_color(x) and x_parent instead.
 * @param x_parent  The parent of x. Always kept valid even when x is NULL.
 */
void delete_fix_up(node_t **root, node_t *x, node_t *x_parent)
{
    node_t *w; /* The sibling of x at each iteration. */

    /*
     * Loop invariant:
     *   - x carries one extra unit of blackness ("double black").
     *   - x_parent is always x's actual parent, even when x is NULL.
     *   - The loop exits when x reaches the root (extra black is simply dropped)
     *     OR when x is RED (can absorb the extra black by becoming BLACK).
     */
    while (x != *root && get_color(x) == BLACK) {

        /* ── x is a LEFT child ─────────────────────────────────────────── */
        if (x == x_parent->left) {
            w = x_parent->right;  /* w is x's sibling (right of parent). */

            /* Case 1: Sibling w is RED.
             * Convert to Case 2/3/4 by rotating left at x_parent. */
            if (get_color(w) == RED) {
                set_color(w, BLACK);
                set_color(x_parent, RED);
                left_rotation(root, x_parent);
                w = x_parent->right; /* w is now the new sibling after rotation. */
            }

            /* Case 2: Sibling w is BLACK, both of w's children are BLACK.
             * Neither nephew can absorb x's extra black. Push the debt upward. */
            if (get_color(w->left) == BLACK && get_color(w->right) == BLACK) {
                set_color(w, RED);    /* w gives one black credit upward. */
                x        = x_parent; /* The double black moves up to x_parent. */
                x_parent = x->parent;
            } else {
                /* Case 3: Sibling w is BLACK, w's RIGHT child is BLACK (left is RED).
                 * Rotate at w to convert to Case 4. */
                if (get_color(w->right) == BLACK) {
                    set_color(w->left, BLACK);
                    set_color(w, RED);
                    right_rotation(root, w);
                    w = x_parent->right; /* w is the new sibling after rotation. */
                }

                /* Case 4: Sibling w is BLACK, w's RIGHT child is RED.
                 * Perform a left rotation at x_parent. This rotation "gives" one
                 * black to x's side, resolving the double black in one step. */
                set_color(w, get_color(x_parent)); /* w inherits x_parent's color. */
                set_color(x_parent, BLACK);
                set_color(w->right, BLACK);
                left_rotation(root, x_parent);
                x = *root; /* Force loop termination — tree is now balanced. */
                break;
            }
        }

        /* ── x is a RIGHT child — mirror image of the left-child cases ─── */
        else {
            w = x_parent->left;  /* w is x's sibling (left of parent). */

            /* Case 1 Mirror: Sibling w is RED. */
            if (get_color(w) == RED) {
                set_color(w, BLACK);
                set_color(x_parent, RED);
                right_rotation(root, x_parent);
                w = x_parent->left;
            }

            /* Case 2 Mirror: Sibling w is BLACK, both of w's children BLACK. */
            if (get_color(w->right) == BLACK && get_color(w->left) == BLACK) {
                set_color(w, RED);
                x        = x_parent;
                x_parent = x->parent;
            } else {
                /* Case 3 Mirror: Sibling w is BLACK, w's LEFT child is BLACK. */
                if (get_color(w->left) == BLACK) {
                    set_color(w->right, BLACK);
                    set_color(w, RED);
                    left_rotation(root, w);
                    w = x_parent->left;
                }

                /* Case 4 Mirror: Sibling w is BLACK, w's LEFT child is RED. */
                set_color(w, get_color(x_parent));
                set_color(x_parent, BLACK);
                set_color(w->left, BLACK);
                right_rotation(root, x_parent);
                x = *root;
                break;
            }
        }
    }

    /*
     * Final step: ensure x is BLACK.
     *
     *  - If we exited the loop because x became RED (absorbed the extra black),
     *    this explicitly makes it BLACK — the correct resolution.
     *  - If we exited because x reached the root, painting the root BLACK
     *    is always correct (Property 2).
     *  - If x is NULL (a NIL leaf), set_color() is a no-op — safe by design.
     */
    set_color(x, BLACK);
}


/* ═══════════════════════════════════════════════════════════════════════════
 * MAIN DELETE FUNCTION
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief  Deletes the node containing key from the Red-Black Tree.
 *
 * See the file header for the full algorithm description. The function
 * handles three structural cases for the target node z:
 *   A. z has no left child.
 *   B. z has no right child.
 *   C. z has two children (uses in-order successor).
 *
 * The global `status` is set by this function:
 *   status = 0 if key was not found (or tree is empty).
 *   status = 1 if key was found and deleted.
 *
 * @param root  The current tree root (may be NULL for an empty tree).
 * @param key   The integer key to delete.
 * @return      The new tree root after deletion (and possible rebalancing).
 *              Returns NULL if the tree is now empty.
 */
node_t *delete_t(node_t *root, int key)
{
    /* Guard: deletion from an empty tree is a no-op. */
    if (root == NULL) {
        status = 0;
        return NULL;
    }

    /* ── Phase 1: Find the target node z ─────────────────────────────────── */
    node_t *z = search_t(root, key);

    if (z == NULL) {
        status = 0;  /* Key not found — tree is unchanged. */
        return root;
    }

    status = 1;  /* Key found — deletion will proceed. */

    /* ── Phase 2: Set up tracking variables ──────────────────────────────── */

    /*
     * y: the node that is physically removed from its current position.
     *    If z has 0 or 1 child, y = z (z is removed directly).
     *    If z has 2 children, y = in-order successor (removed from its position
     *    and then moved up to replace z).
     */
    node_t *y = z;
    color_t y_original_color = y->color; /* Saved BEFORE any transplants. */

    /*
     * x: the node that moves into y's vacated position.
     *    May be NULL when y is a leaf (its replacement is a NIL sentinel).
     *
     * x_parent: tracks x's parent separately because x may be NULL,
     *           and NULL->parent is undefined behavior.
     */
    node_t *x;
    node_t *x_parent;

    /* ── Phase 3: BST Structural Deletion (three cases) ──────────────────── */

    /* Case A: z has NO LEFT child.
     * Replace z directly with its right child (which may be NULL). */
    if (z->left == NULL) {
        x        = z->right;
        x_parent = z->parent;  /* x_parent is z's parent (x may be NULL). */
        transplant(&root, z, z->right);
    }

    /* Case B: z has NO RIGHT child (but has a left child).
     * Replace z directly with its left child. */
    else if (z->right == NULL) {
        x        = z->left;
        x_parent = z->parent;
        transplant(&root, z, z->left);
    }

    /* Case C: z has BOTH children.
     * Find y = the in-order successor (minimum of z's right subtree).
     * The in-order successor has at most a right child (no left child by definition).
     * Splice y out of its current position, then move y into z's position. */
    else {
        y                = find_min(z->right); /* In-order successor. */
        y_original_color = y->color;
        x                = y->right;  /* y's only possible child. */

        if (y->parent == z) {
            /*
             * Special sub-case: y is z's DIRECT right child.
             * x_parent cannot be y->parent (z, about to be freed).
             * Set x_parent = y so that after z is freed, x_parent remains valid.
             */
            x_parent = y;
        } else {
            /*
             * General sub-case: y is deeper in z's right subtree.
             * 1. Splice y out: replace y with y's right child (x).
             * 2. Reattach y's right pointer to z's right subtree.
             */
            x_parent = y->parent;
            transplant(&root, y, y->right);  /* Step 1: splice y out. */
            y->right         = z->right;     /* Step 2a: y adopts z's right. */
            y->right->parent = y;
        }

        /* Move y into z's position. */
        transplant(&root, z, y);
        y->left         = z->left;   /* y adopts z's left subtree. */
        y->left->parent = y;
        y->color        = z->color;  /* y assumes z's color to preserve black-height. */
    }

    /* Free the target node z — it is now fully detached from the tree. */
    free(z);

    /* ── Phase 4: Conditional Fix-Up ─────────────────────────────────────── */

    /*
     * A BLACK node was physically removed from the tree (y's original color).
     * This reduces the black-height of all paths through y's former position
     * by 1 — a Double Black violation. Repair it now.
     *
     * If y was RED, the black-heights are unchanged and no fix is needed.
     */
    if (y_original_color == BLACK) {
        delete_fix_up(&root, x, x_parent);
    }

    return root;
}