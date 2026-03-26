#include "tree.h"

int find_min(node_t *root)
{
    if(root == NULL)
        return NULL;
    
    node_t *temp = root;
    while(temp->left)
    {
        temp = temp->left;
    }

    return temp->data;
}