#include "tree.h"

// Display function matching project sample output
void display_tree(node_t *root) {
    // Base case: if we hit a NULL pointer (NIL leaf), just stop and go back up
    if (root != NULL) {
        // 1. Traverse the left subtree (find the smallest numbers first)
        display_tree(root->left);
        
        // 2. Print the current node's data and its color value
        if (root->color == RED) {
            printf("(%d)--(RED->0)  ", root->data);
        } else {
            printf("(%d)--(BLACK->1)  ", root->data);
        }
        
        // 3. Traverse the right subtree (move on to the larger numbers)
        display_tree(root->right);
    }
}