#include "tree.h"

int find_max(node_t *root)
{
    if(root == NULL)
        return NULL;
    
    node_t *temp = root;
    while(temp->right)
    {
        temp = temp->right;
    }

    return temp->data;
}