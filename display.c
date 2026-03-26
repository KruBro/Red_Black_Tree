/**
 * @file    display.c
 * @brief   In-order traversal rendering of the Red-Black Tree with ANSI colors.
 *
 * Performs a Left -> Node -> Right (LNR) in-order traversal of the tree and
 * prints each node to stdout. Because in-order traversal of a BST always
 * visits nodes in ascending key order, the output is a sorted sequence of all
 * stored values -- a useful correctness check after insertions or deletions.
 *
 * OUTPUT FORMAT
 *   Each node is printed as:  (data)--(COLOR->value)
 *   where value matches the color_t enum: RED = 0, BLACK = 1.
 *
 *   Example output for {10, 15, 20, 25, 30}:
 *     (10)--(BLACK->1)  (15)--(RED->0)  (20)--(BLACK->1)  ...
 *
 *   With ANSI colors (color.h), RED nodes appear in bold red terminal text
 *   and BLACK nodes appear in gray. COLOR_RESET is applied after every node
 *   to prevent color bleeding into the next node or the shell prompt.
 *
 * WHY IN-ORDER?
 *   Pre-order (NLR) would show hierarchical structure better, but in-order
 *   was chosen to match the project specification output format and to give
 *   an immediate visual confirmation that BST ordering is intact.
 *
 * @see color.h  for CLR_NODE_RED, CLR_NODE_BLACK, COLOR_RESET definitions
 * @see tree.h   for node_t definition and prototype
 */

#include "tree.h"
#include "color.h"


/**
 * @brief  Prints all nodes in ascending key order with ANSI color coding.
 *
 * Recursive in-order (LNR) traversal:
 *   1. Recurse into the left subtree  (smaller keys first).
 *   2. Print the current node with appropriate color.
 *   3. Recurse into the right subtree (larger keys last).
 *
 * Terminates at NULL pointers (NIL leaves) -- the base case returns
 * without printing, which correctly handles empty trees and leaf edges.
 *
 * Color-coding:
 *   RED nodes   -> CLR_NODE_RED   (bold red terminal text)
 *   BLACK nodes -> CLR_NODE_BLACK (gray terminal text)
 *   COLOR_RESET applied after each node to restore terminal defaults.
 *
 * @param root  Root of the (sub)tree to display. NULL is safe -- no output.
 */
void display_tree(node_t *root)
{
    /*
     * Base case: NULL is a NIL leaf -- nothing to print, nothing to recurse.
     * Also handles the empty-tree case when called from main.c.
     */
    if (root != NULL) {

        /* Step 1: Traverse left subtree (all keys smaller than root->data). */
        display_tree(root->left);

        /* Step 2: Print this node. */
        if (root->color == RED) {
            /*
             * RED node: bold red text so it stands out against BLACK nodes.
             * Enum value RED = 0, matching the "RED->0" label.
             */
            printf(CLR_NODE_RED "(%d)--(RED->0)" COLOR_RESET "  ", root->data);
        } else {
            /*
             * BLACK node: gray text for contrast without competing with red.
             * Enum value BLACK = 1, matching the "BLACK->1" label.
             */
            printf(CLR_NODE_BLACK "(%d)--(BLACK->1)" COLOR_RESET "  ", root->data);
        }

        /* Step 3: Traverse right subtree (all keys larger than root->data). */
        display_tree(root->right);
    }
}