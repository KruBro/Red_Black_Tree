#include "tree.h"

color_t recolor(node_t *node)
{
    if(node->color == RED)
        return BLACK;
    else
        return RED;
}

Status balance_t(node_t **root, node_t *new_node) {
    // Safety Check 1: Is this the root?
    if (new_node->parent == NULL) {
        new_node->color = BLACK; 
        return SUCCESS;     
    }

    // If parent is BLACK, no rules are broken
    if (new_node->parent->color == BLACK) {
        return SUCCESS;
    }

    // Safety Check 2: Does grandparent exist?
    if (new_node->parent->parent == NULL) {
        return SUCCESS; 
    }

    node_t *uncle = NULL;
    node_t *parent = new_node->parent;
    node_t *grand_parent = parent->parent;

    // Find the uncle
    if (parent == grand_parent->left) {
        uncle = grand_parent->right;
    } else {
        uncle = grand_parent->left;
    }

    // CASE: Uncle is RED
    if (uncle != NULL && uncle->color == RED) {
        uncle->color = recolor(uncle);
        grand_parent->color = recolor(grand_parent);
        parent->color = recolor(parent);
        
        // Recurse upwards
        return balance_t(root, grand_parent);
    }
    // CASE: Uncle is BLACK (or NULL)
    else {
        rotate(root, new_node);
        // Force the absolute root to be black just in case rotations moved a red node to the top
        (*root)->color = BLACK; 
        return SUCCESS;
    }
}