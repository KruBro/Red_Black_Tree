/**
 * @file    insertion.c
 * @brief   Red-Black Tree insertion and post-insertion balancing.
 *
 * This file contains two cooperating functions:
 *
 *   insert()    — Allocates a new node, places it using standard BST rules,
 *                 then calls balance_t() to restore Red-Black properties.
 *
 *   balance_t() — Walks up the tree from the new node, resolving Red-Red
 *                 violations (Property 3) through recoloring and/or rotation.
 *
 * ─── INSERT ALGORITHM OVERVIEW ────────────────────────────────────────────
 *
 *  Step 1: Allocate and initialize the new node (colored RED).
 *          Newly inserted nodes start as RED because inserting a RED node
 *          never changes the black-height of any path (Property 4 is safe).
 *          It may, however, create a Red-Red violation (Property 3), which
 *          balance_t() then resolves.
 *
 *  Step 2: If the tree is empty, paint the new node BLACK and make it root.
 *          (Property 2: root must be BLACK.)
 *
 *  Step 3: Walk down the tree using standard BST comparison to find the
 *          correct insertion point. Reject duplicate keys.
 *
 *  Step 4: Link the new node to its parent and call balance_t().
 *
 * ─── BALANCE ALGORITHM OVERVIEW ───────────────────────────────────────────
 *
 *  A Red-Red violation occurs when the new node (RED) has a RED parent.
 *  The fix depends on the color of the uncle (parent's sibling):
 *
 *  Uncle RED → Recolor: flip parent, uncle, and grandparent colors.
 *              The grandparent may now violate Property 3 with ITS parent,
 *              so we recurse upward from the grandparent.
 *
 *  Uncle BLACK (or NULL) → Rotate: determine the structural shape
 *              (LL, RR, LR, RL) and dispatch to rotate().
 *              Rotations fix the violation locally without propagating upward.
 *
 *  In both cases, the root is explicitly set to BLACK afterward to maintain
 *  Property 2 — recoloring may temporarily make the root RED.
 *
 * @see rotation.c  for the implementation of rotate(), left_rotation(), right_rotation()
 * @see tree.h      for the node_t definition and function prototypes
 */

#include "tree.h"


/**
 * @brief  Inserts a new integer key into the Red-Black Tree.
 *
 * Combines standard iterative BST insertion with RB-Tree rebalancing.
 * The caller only needs to pass the root pointer and the key — all
 * allocation, linking, and balancing are handled internally.
 *
 * @param root  Double pointer to the root. Needed because insertion into an
 *              empty tree (or rotations during rebalancing) may change which
 *              node is the root.
 * @param data  The integer key to insert.
 * @return      SUCCESS  — node inserted and tree is balanced.
 *              FAILURE  — malloc returned NULL, OR data is already in the tree
 *                         (duplicate keys are not allowed; the tree is unchanged).
 *
 * @note  The newly allocated node starts as RED. This is a deliberate choice:
 *        inserting RED preserves black-height (Property 4) at the cost of
 *        possibly violating the Red-Red rule (Property 3), which balance_t fixes.
 */
Status insert(node_t **root, int data)
{
    /* Guard: the root pointer itself must be valid. */
    if (root == NULL)
        return FAILURE;

    /* ── Step 1: Allocate and initialize the new node ────────────────────── */
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
        return FAILURE;  /* malloc failure — tree is unchanged. */

    new->data   = data;
    new->left   = NULL;   /* New node has no children (it's a leaf). */
    new->right  = NULL;
    new->parent = NULL;
    new->color  = RED;    /* Always insert RED — see file header for rationale. */

    /* ── Step 2: Handle the empty-tree special case ───────────────────────── */
    if (*root == NULL) {
        new->color = BLACK;  /* Root MUST be BLACK (Property 2). */
        *root = new;
        return SUCCESS;
    }

    /* ── Step 3: Standard iterative BST descent to find insertion point ──── */
    node_t *temp = *root;   /* 'temp' walks down to the insertion position.   */
    node_t *prev = NULL;    /* 'prev' trails one step behind to become parent. */

    while (temp) {
        prev = temp;  /* Record the last valid node before stepping off a leaf. */

        if (data < temp->data) {
            temp = temp->left;   /* Key is smaller — go left. */
        } else if (data > temp->data) {
            temp = temp->right;  /* Key is larger — go right. */
        } else {
            /*
             * Duplicate key found. Free the pre-allocated node and return
             * FAILURE. The tree is left completely unchanged.
             */
            free(new);
            return FAILURE;
        }
    }
    /* After the loop: prev is the future parent, temp is NULL (a NIL leaf). */

    /* ── Step 4: Link new node into the tree ─────────────────────────────── */
    new->parent = prev;

    if (data < prev->data)
        prev->left = new;   /* Insert as left child. */
    else
        prev->right = new;  /* Insert as right child. */

    /* ── Step 5: Restore Red-Black properties ────────────────────────────── */
    balance_t(root, new);
    return SUCCESS;
}


/**
 * @brief  Restores Red-Black Property 3 after insertion (recursive).
 *
 * This function is called from insert() on the newly placed node, and may
 * call itself recursively on an ancestor when a recoloring step pushes the
 * violation upward.
 *
 * ─── CASE ANALYSIS ────────────────────────────────────────────────────────
 *
 *  Before any case: a Red-Red violation exists between new_node and its parent.
 *  Let:  P = new_node->parent
 *        G = P->parent   (grandparent)
 *        U = sibling of P (uncle)
 *
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │ CASE A — Uncle is RED                                               │
 *  │                                                                     │
 *  │    G(B)            G(R)  ← may violate P3 with G's parent         │
 *  │   /   \    →      /   \                                             │
 *  │  P(R)  U(R)      P(B)  U(B)                                        │
 *  │  |                |                                                 │
 *  │  N(R)             N(R)  ← violation resolved locally               │
 *  │                                                                     │
 *  │  Action: Recolor P→BLACK, U→BLACK, G→RED.                          │
 *  │          Recurse upward with G as the new_node.                     │
 *  └─────────────────────────────────────────────────────────────────────┘
 *
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │ CASE B — Uncle is BLACK (or NULL)                                   │
 *  │                                                                     │
 *  │  The structural shape of (G, P, N) determines which rotation:       │
 *  │    LL, RR, LR, or RL — handled by rotate() in rotation.c.           │
 *  │                                                                     │
 *  │  Rotations fix the violation in O(1) steps with no upward recursion.│
 *  └─────────────────────────────────────────────────────────────────────┘
 *
 * @param root      Double pointer to the tree root (may change after rotation).
 * @param new_node  The node currently being examined for a Red-Red violation.
 *                  On the initial call this is the newly inserted node;
 *                  on recursive calls it is a grandparent after recoloring.
 */
void balance_t(node_t **root, node_t *new_node)
{
    /* ── Base Case 1: We have reached the root ────────────────────────────── */
    if (new_node->parent == NULL) {
        /*
         * The root must always be BLACK (Property 2).
         * This handles the case where recoloring propagated all the way up
         * and temporarily made the root RED.
         */
        new_node->color = BLACK;
        return;
    }

    /* ── Base Case 2: Parent is BLACK — no violation ─────────────────────── */
    if (new_node->parent->color == BLACK) {
        /*
         * A RED new_node with a BLACK parent is perfectly valid.
         * Property 3 is satisfied; nothing to fix.
         */
        return;
    }

    /* ── Base Case 3: Parent has no grandparent ──────────────────────────── */
    if (new_node->parent->parent == NULL) {
        /*
         * The parent IS the root. The root is always BLACK (Property 2),
         * so if we reach here with a RED parent, the parent will be corrected
         * by the root-check in the next recursion or by the final (*root)->color
         * assignment in the caller. Return safely.
         */
        return;
    }

    /* ── Setup: identify parent, grandparent, and uncle ─────────────────── */
    node_t *parent       = new_node->parent;
    node_t *grand_parent = parent->parent;
    node_t *uncle        = NULL;

    /*
     * Determine uncle: the sibling of the parent.
     * If parent is the left child, uncle is the right child of grandparent,
     * and vice versa. Uncle may be NULL (a NIL leaf), which is BLACK.
     */
    if (parent == grand_parent->left) {
        uncle = grand_parent->right;
    } else {
        uncle = grand_parent->left;
    }

    /* ── CASE A: Uncle is RED → Recolor and recurse upward ───────────────── */
    if (uncle != NULL && uncle->color == RED) {
        /*
         * Flip the colors of parent, uncle, and grandparent.
         * recolor() returns the toggled color — it does NOT mutate the node.
         *
         *  P(R) → P(B)   (fixes the Red-Red violation between new_node and P)
         *  U(R) → U(B)   (symmetry — uncle must match parent's new color)
         *  G(B) → G(R)   (G gives its BLACK to P and U)
         *
         * G is now RED. If G's parent is also RED, we have pushed the
         * violation one level up. Recurse from G to resolve it.
         */
        uncle->color       = recolor(uncle);
        grand_parent->color = recolor(grand_parent);
        parent->color       = recolor(parent);

        balance_t(root, grand_parent);  /* Recurse upward from grandparent. */
    }

    /* ── CASE B: Uncle is BLACK (or NULL) → Rotate ───────────────────────── */
    else {
        /*
         * rotate() inspects the LL/RR/LR/RL shape of (grand_parent, parent,
         * new_node) and performs the appropriate single or double rotation,
         * including the necessary recoloring inside rotation.c.
         */
        rotate(root, new_node);

        /*
         * Force the absolute root to BLACK.
         * A rotation may have elevated a RED node to the root position.
         * Property 2 requires an explicit correction here.
         */
        (*root)->color = BLACK;
        return;
    }
}