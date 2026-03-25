#include "tree.h"

void left_rotation(node_t **root, node_t *x)
{
    if (x == NULL || x->right == NULL)
        return;

    node_t *y = x->right;

    // Move y's left subtree to x's right
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;

    // Link y with x's parent
    y->parent = x->parent;

    if (x->parent == NULL)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // Put x as left child of y
    y->left = x;
    x->parent = y;
}

void right_rotation(node_t **root, node_t *x)
{
    if (x == NULL || x->left == NULL)
        return;

    node_t *y = x->left;

    // Move y's right subtree to x's left
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;

    // Link y with x's parent
    y->parent = x->parent;

    if (x->parent == NULL)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // Put x as right child of y
    y->right = x;
    x->parent = y;
}

void rotate(node_t **root, node_t *n)
{
    node_t *p = n->parent;
    if (p == NULL || p->parent == NULL) return; // Safety check
    
    node_t *g = p->parent;

    // Case LL: n is left child of p, and p is left child of g
    if (n == p->left && p == g->left)
    {
        // Implementation for Right Rotation on g goes here
        right_rotation(root, g);
        g->color = recolor(g);
        p->color = recolor(p);
    }
    // Case RR: n is right child of p, and p is right child of g
    else if(n == p->right && p == g->right)
    {
        left_rotation(root, g);
        g->color = recolor(g);
        p->color = recolor(p);        
    }
    // Case LR: n is right child of p, and p is left child of g
    else if(n == p->right && p == g->left)
    {
        left_rotation(root, p);
        right_rotation(root, g);

        n->color = recolor(n);
        g->color = recolor(g);
    }
    // Case RL: n is left child of p, and p is right child of g
    else if(n == p->left && p == g->right)
    {
        right_rotation(root, p);
        left_rotation(root, g);

        n->color = recolor(n);
        g->color = recolor(g);
    }
}
