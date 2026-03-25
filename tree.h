#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h> 

typedef enum{
    RED,
    BLACK
} color_t;

typedef enum{
    SUCCESS,
    FAILURE
} Status;

typedef struct node{
    int data;
    color_t color;
    struct node *left;
    struct node *right;
    struct node *parent;
} node_t;

void display_tree(node_t *root);

Status insert(node_t **root, int data);
color_t recolor(node_t *node);
Status balance_t(node_t **root, node_t *new_node);

void left_rotation(node_t **root, node_t *x);
void right_rotation(node_t **root, node_t *x);
void rotate(node_t **root, node_t *n);

#endif