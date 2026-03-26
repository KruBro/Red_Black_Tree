#include "tree.h"

Status search_t(node_t *root, int key)
{    
    node_t *temp = root;
    
    while(temp)
    {
        if(temp->data == key)
            return SUCCESS;
        else if(temp->data > key)
            temp = temp->left;
        else
            temp = temp->right;
    }

    return DATA_NOT_FOUND;
}