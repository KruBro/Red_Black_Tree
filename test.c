#include "tree.h"
#include <assert.h>
#include <stdlib.h>

int main() {
    node_t *root = NULL;
    
    printf("--- Running Red-Black Tree Unit Tests ---\n");
    
    // 1. Insert 10 (Becomes Black Root)
    insert(&root, 10);
    assert(root != NULL);
    assert(root->data == 10);
    assert(root->color == BLACK);
    printf("[PASS] Insert Root (10)\n");

    // 2. Insert 20 (Becomes Red Right Child)
    insert(&root, 20);
    assert(root->right->data == 20);
    assert(root->right->color == RED);
    printf("[PASS] Insert Right Child (20)\n");

    // 3. Insert 30 (Triggers RR violation -> Left Rotation on 10)
    // After rotation: 20 should be the new BLACK root. 10 is Left, 30 is Right.
    insert(&root, 30);
    assert(root->data == 20);
    assert(root->color == BLACK);
    assert(root->left->data == 10);
    assert(root->left->color == RED);
    assert(root->right->data == 30);
    assert(root->right->color == RED);
    printf("[PASS] Left Rotation / RR Fix (30)\n");

    printf("--- All Unit Tests Passed Successfully! ---\n");
    
    return 0;
}