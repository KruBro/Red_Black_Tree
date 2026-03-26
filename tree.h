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
    FAILURE,
    DATA_NOT_FOUND,
    TREE_EMPTY
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
Status search_t(node_t *root, int key);
int find_min(node_t *root);
int find_max(node_t *root);

color_t recolor(node_t *node);
void balance_t(node_t **root, node_t *new_node);

void left_rotation(node_t **root, node_t *x);
void right_rotation(node_t **root, node_t *x);
void rotate(node_t **root, node_t *n);

#endif