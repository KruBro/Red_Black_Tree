#include "tree.h"

Status insert(node_t **root, int data)
{
    if (root == NULL)
        return FAILURE;

    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
        return FAILURE;

    new->data = data;
    new->left = NULL;
    new->right = NULL;
    new->parent = NULL;
    new->color = RED;

    if (*root == NULL)
    {
        new->color = BLACK;
        *root = new;
        return SUCCESS;
    }

    node_t *temp = *root, *prev = NULL;

    while (temp)
    {
        prev = temp;

        if (data < temp->data)
            temp = temp->left;
        else if (data > temp->data)
            temp = temp->right;
        else
        {
            free(new);
            return FAILURE;
        }
    }

    new->parent = prev;

    if (data < prev->data)
        prev->left = new;
    else
        prev->right = new;

    balance_t(root, new);
    return SUCCESS;
}