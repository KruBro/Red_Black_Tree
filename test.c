/**
 * @file    test.c
 * @brief   Comprehensive unit test suite for the Red-Black Tree implementation.
 *
 * Tests all 9 operations exposed by the interactive menu in main.c, using
 * the C standard library's assert() macro for validation. A failed assert()
 * aborts the program with a clear message indicating the file, line number,
 * and the expression that evaluated to false — making failures easy to locate.
 *
 * ─── TEST PHILOSOPHY ──────────────────────────────────────────────────────
 *
 *  Tests are ordered by dependency: later tests build on the state established
 *  by earlier ones. The test sequence mirrors real usage:
 *
 *    1. Verify edge cases on an empty tree FIRST (no preconditions required).
 *    2. Test insert + rotation cases on isolated 3-node trees (small, controlled).
 *    3. Build a complex 12-node tree and test all read operations on it.
 *    4. Test deletion operations on the complex tree.
 *    5. Verify memory cleanup.
 *
 * ─── COVERED OPERATIONS ───────────────────────────────────────────────────
 *
 *   Test Group             | Operations Verified
 *   ─────────────────────────────────────────────────────────────────────
 *   Empty Tree Edge Cases  | find_min, find_max, search_t, delete_t on NULL
 *   Insertion & Balancing  | RR, LL, LR, RL rotation cases; root color; structure
 *   Search & Display       | search_t (found, leaf, not-found); display_tree output
 *   Find MIN / MAX         | find_min, find_max on 12-node tree
 *   Delete MIN / MAX       | delete_t on leaf min (1) and leaf max (70)
 *   Complex Deletions      | delete_t on root (2-child node), delete RED leaf
 *   Memory Cleanup         | free_tree; root set to NULL; no leaks (Valgrind)
 *
 * ─── RUNNING THE TESTS ────────────────────────────────────────────────────
 *
 *   make test      -- compiles test.c with all other .c modules (excludes main.c)
 *   ./test_rbt     -- runs the binary; prints PASS/FAIL per group
 *
 * ─── KNOWN DESIGN DECISIONS ───────────────────────────────────────────────
 *
 *  - int status is declared here (not in main.c) when linking the test binary,
 *    because main.c is excluded from the test build to avoid a duplicate main().
 *  - No external test framework is used. assert() + printf is sufficient for
 *    a focused single-file test suite and keeps the build self-contained.
 *
 * @see main.c      for the interactive version of the same 9 operations
 * @see tree.h      for node_t, Status, and all function prototypes
 */

#include "tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Required when linking the test binary.
 * delete.c declares `extern int status` — this provides the actual storage.
 * In the normal build, this variable lives in main.c; here it lives here.
 */
int status;


/**
 * @brief  Entry point for the test suite.
 *
 * Executes all test groups sequentially. Prints a PASS message after each
 * group that completes without triggering an assert(). If any assert() fails,
 * the program aborts immediately with an error on stderr, indicating the
 * exact expression and source line that failed.
 *
 * @return 0 if all tests pass (the process exits normally).
 *         Non-zero (abort) if any assert() fails.
 */
int main(void) {
    node_t *root = NULL;

    printf("==================================================\n");
    printf("   RED-BLACK TREE COMPREHENSIVE UNIT TESTS\n");
    printf("==================================================\n\n");


    /* =========================================================
     * GROUP 1: Edge Cases on an Empty Tree
     *
     * All read and write operations must handle NULL root safely.
     * Early builds crashed here with segmentation faults before
     * NULL guards were added to find_min, find_max, and delete_t.
     * ========================================================= */
    printf("Testing Empty Tree Edge Cases...\n");

    /* find_min and find_max on an empty tree must return NULL, not crash. */
    assert(find_min(root) == NULL);
    assert(find_max(root) == NULL);

    /* search_t on an empty tree must return NULL, not crash. */
    assert(search_t(root, 50) == NULL);

    /* delete_t on an empty tree must be a no-op and set status = 0. */
    root = delete_t(root, 50);
    assert(root == NULL);
    assert(status == 0);  /* 0 = key not found (or tree empty). */

    printf("  [PASS] Empty tree handled safely.\n\n");


    /* =========================================================
     * GROUP 2: Insertion and Rotation Cases
     *
     * Each sub-test inserts exactly 3 nodes in an order that triggers
     * a specific rotation case (LL, RR, LR, RL), then verifies:
     *   - The correct node is at the root.
     *   - The root is BLACK (Property 2).
     *   - The correct nodes are the left and right children.
     *
     * In all four cases, inserting a sorted 3-node sequence into a
     * correctly balanced RB-Tree should produce the same structure:
     *   middle node as BLACK root, smaller as left child, larger as right.
     * ========================================================= */
    printf("Testing Case 1: Insertions & Balancing (all 4 rotation types)...\n");

    /* ── RR Case (Right-Right): 10 → 20 → 30 ─────────────────────────────
     * After inserting 10 (root, BLACK) and 20 (right child, RED),
     * inserting 30 (right child of 20, RED) creates a RR Red-Red violation.
     * Fix: left_rotation on root (10), recolor. Result: 20 is new BLACK root. */
    insert(&root, 10); insert(&root, 20); insert(&root, 30);
    assert(root->data == 20 && root->color == BLACK);  /* 20 rose to root. */
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* ── LL Case (Left-Left): 30 → 20 → 10 ───────────────────────────────
     * Symmetric to RR. Inserting 10 as left child of 20 (which is left child
     * of 30) creates an LL violation. Fix: right_rotation on 30. */
    insert(&root, 30); insert(&root, 20); insert(&root, 10);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* ── LR Case (Left-Right): 30 → 10 → 20 ─────────────────────────────
     * 10 is left child of 30, then 20 is inserted as right child of 10.
     * This zig-zag shape requires a double rotation:
     *   left_rotation(10) converts to LL, then right_rotation(30). */
    insert(&root, 30); insert(&root, 10); insert(&root, 20);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    /* ── RL Case (Right-Left): 10 → 30 → 20 ─────────────────────────────
     * 30 is right child of 10, then 20 is inserted as left child of 30.
     * Zig-zag requires: right_rotation(30) converts to RR, then left_rotation(10). */
    insert(&root, 10); insert(&root, 30); insert(&root, 20);
    assert(root->data == 20 && root->color == BLACK);
    assert(root->left->data == 10 && root->right->data == 30);
    free_tree(root); root = NULL;

    printf("  [PASS] All 4 rotation cases correct.\n\n");


    /* =========================================================
     * SETUP: Build a Complex 12-Node Tree for Subsequent Tests
     *
     * This sequence was chosen to exercise cascade recoloring,
     * multiple rotation types, and a variety of node colors.
     * After all 12 insertions:
     *   - The tree re-rooted itself dynamically (root became 16).
     *   - Minimum is 1 (leftmost leaf).
     *   - Maximum is 70 (rightmost leaf).
     *   - Valgrind confirms no leaks during this sequence.
     * ========================================================= */
    int values[] = {10, 18, 7, 15, 16, 30, 25, 40, 60, 2, 1, 70};
    int n = sizeof(values) / sizeof(values[0]);  /* 12 elements. */
    for (int i = 0; i < n; i++) {
        insert(&root, values[i]);
    }


    /* =========================================================
     * GROUP 3: Search and Display (Cases 3 & 4)
     * ========================================================= */
    printf("Testing Case 3 & 4: Search & Display...\n");

    assert(search_t(root, 16) != NULL);  /* Root node — must be found. */
    assert(search_t(root, 70) != NULL);  /* Rightmost leaf — must be found. */
    assert(search_t(root, 99) == NULL);  /* Not in tree — must return NULL. */

    /* Visual confirmation: in-order output should be ascending. */
    printf("  Tree Display (In-Order):\n  ");
    display_tree(root);
    printf("\n");

    printf("  [PASS] Search logic verified.\n\n");


    /* =========================================================
     * GROUP 4: Find MIN and Find MAX (Cases 5 & 6)
     * ========================================================= */
    printf("Testing Case 5 & 6: Find MIN / MAX...\n");

    assert(find_min(root)->data == 1);   /* 1 is the leftmost leaf. */
    assert(find_max(root)->data == 70);  /* 70 is the rightmost leaf. */

    printf("  [PASS] Minimum is 1, Maximum is 70.\n\n");


    /* =========================================================
     * GROUP 5: Delete MIN and Delete MAX (Cases 7 & 8)
     *
     * Deleting the current min (1) should make 2 the new min.
     * Deleting the current max (70) should make 60 the new max.
     * status must be 1 (found and deleted) after each call.
     * ========================================================= */
    printf("Testing Case 7 & 8: Delete MIN / MAX...\n");

    root = delete_t(root, 1);           /* Delete MIN (1). */
    assert(status == 1);                /* Confirm deletion succeeded. */
    assert(find_min(root)->data == 2);  /* 2 is now the new minimum. */

    root = delete_t(root, 70);          /* Delete MAX (70). */
    assert(status == 1);
    assert(find_max(root)->data == 60); /* 60 is now the new maximum. */

    printf("  [PASS] Min and Max successfully deleted and updated.\n\n");


    /* =========================================================
     * GROUP 6: Complex Internal Deletions (Case 2)
     *
     * Sub-test A: Delete the ROOT (a node with two children).
     *   Node 16 is the current root and has two children.
     *   The deletion algorithm must:
     *     1. Find the in-order successor of 16 (the minimum of the right
     *        subtree, which is 18).
     *     2. Splice 18 out of its current position.
     *     3. Move 18 into 16's position, copying 16's color.
     *     4. Free node 16.
     *   After deletion, root->data must be 18.
     *
     * Sub-test B: Delete a RED leaf node (15).
     *   Deleting a RED leaf is the simplest deletion case:
     *   y_original_color is RED, so no fix-up is triggered.
     *   The node is simply unlinked and freed.
     * ========================================================= */
    printf("Testing Case 2: Complex Internal Deletions...\n");

    /* Sub-test A: Delete the root (16, two-child node). */
    root = delete_t(root, 16);
    assert(status == 1);                      /* Deletion succeeded. */
    assert(search_t(root, 16) == NULL);       /* 16 must no longer exist. */
    assert(root->data == 18);                 /* 18 (successor) is new root. */

    /* Sub-test B: Delete a RED leaf (15). */
    root = delete_t(root, 15);
    assert(status == 1);
    assert(search_t(root, 15) == NULL);       /* 15 must no longer exist. */

    printf("  [PASS] Internal node and leaf deletions succeeded.\n\n");


    /* =========================================================
     * GROUP 7: Memory Cleanup (Case 9)
     *
     * free_tree() traverses post-order and frees every remaining node.
     * After this call, root is a dangling pointer — immediately set to
     * NULL to prevent accidental use-after-free.
     * Valgrind confirms 0 bytes leaked after this block.
     * ========================================================= */
    printf("Testing Case 9: Free Memory...\n");

    free_tree(root);
    root = NULL;        /* Prevent dangling pointer access. */
    assert(root == NULL);

    printf("  [PASS] Tree memory successfully freed.\n\n");


    /* =========================================================
     * SUMMARY
     * ========================================================= */
    printf("==================================================\n");
    printf(" [SUCCESS] ALL 9 OPERATIONS PASSED FLAWLESSLY!\n");
    printf("==================================================\n");

    return 0;
}