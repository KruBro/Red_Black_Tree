#include "tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    node_t *root = NULL;

    printf("--- Running Extended Red-Black Tree Unit Tests ---\n");

    int values[] = {10, 18, 7, 15, 16, 30, 25, 40, 60, 2, 1, 70};
    int n = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < n; i++) {
        insert(&root, values[i]);
    }

    // --- Final Structural Assertions ---
    // After inserting 40, a major RR rotation pulls 16 to the very top.
    assert(root != NULL);
    assert(root->data == 16);
    assert(root->color == BLACK);

    // Check Left Subtree Root (10)
    // Recolored to BLACK during the insertion of 70 (Uncle RED cascade)
    assert(root->left->data == 10);
    assert(root->left->color == BLACK); 

    // Check Right Subtree Root (25)
    // Recolored to BLACK during the insertion of 70
    assert(root->right->data == 25);
    assert(root->right->color == BLACK); 

    // Check the leftmost leaf (1)
    // Path: 16(L) -> 10(L) -> 2(L) -> 1
    node_t *leftmost = root->left->left->left;
    assert(leftmost != NULL);
    assert(leftmost->data == 1);
    assert(leftmost->color == RED);

    // Check the rightmost leaf (70)
    // Due to right-side rotations, 70 is exactly 4 steps down from the root
    // Path: 16(R) -> 25(R) -> 40(R) -> 60(R) -> 70
    node_t *rightmost = root->right->right->right->right;
    assert(rightmost != NULL);
    assert(rightmost->data == 70);
    assert(rightmost->color == RED);

    // Let's also verify the middle-right nodes to ensure the RR rotations worked
    assert(root->right->right->data == 40);
    assert(root->right->right->color == RED);
    
    assert(root->right->right->right->data == 60);
    assert(root->right->right->right->color == BLACK);

    display_tree(root);
    printf("\n");

    printf("[PASS] Complex sequence (10, 18, 7, 15, 16, 30, 25, 40, 60, 2, 1, 70)\n");

    printf("Testing Search Function......\n");

    assert((search_t(root, 30)) == SUCCESS);
    assert((search_t(root, 24)) == DATA_NOT_FOUND);

    printf("Testing Fin Min and Find Max...\n");

    assert((find_max(root)) == 70);
    assert((find_min(root)) == 1);
    
    printf("--- All Unit Tests Passed Successfully! ---\n");

    return 0;
}