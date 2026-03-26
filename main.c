/**
 * @file    main.c
 * @brief   Interactive terminal menu and entry point for the Red-Black Tree.
 *
 * Provides a do-while menu loop that exposes all 9 supported tree operations
 * to the user. Each menu case validates state (e.g., empty tree), calls the
 * appropriate function from the other modules, and prints a color-coded
 * result message using the macros defined in color.h.
 *
 * GLOBAL STATE
 *   int status  -- Set by delete_t() in delete.c to communicate whether the
 *                  deletion found and removed the key (status=1) or not
 *                  (status=0). It is declared here and referenced as extern
 *                  in delete.c. Using a global avoids changing delete_t()'s
 *                  return type (it already returns the new root pointer).
 *
 * MENU OPERATIONS
 *   1. Insert       -- Adds a key; reports success or duplicate/malloc failure.
 *   2. Delete       -- Removes a key; reports success or not-found.
 *   3. Search       -- Finds a key; reports found or not-found.
 *   4. Display      -- In-order traversal with ANSI color output.
 *   5. Find MIN     -- Reports the smallest key in the tree.
 *   6. Find MAX     -- Reports the largest key in the tree.
 *   7. Delete MIN   -- Finds and removes the minimum key.
 *   8. Delete MAX   -- Finds and removes the maximum key.
 *   9. Exit         -- Calls free_tree() to release all heap memory, then exits.
 *
 * IMPORTANT: Cases 7 and 8 capture the target value BEFORE calling delete_t().
 *   The original code read min->data AFTER delete_t() freed the node, which
 *   is undefined behavior (use-after-free). The corrected version saves the
 *   integer value first, then deletes.
 *
 * @see tree.h      for node_t, Status, and all function prototypes
 * @see color.h     for CLR_* and COLOR_RESET macros
 * @see insertion.c for insert()
 * @see delete.c    for delete_t() and the `status` extern
 * @see search.c    for search_t()
 * @see display.c   for display_tree()
 * @see find_min.c  for find_min()
 * @see find_max.c  for find_max()
 * @see tree_utils.c for free_tree()
 */

#include "tree.h"
#include "color.h"

/**
 * Global deletion status flag.
 * Written by delete_t() in delete.c (declared extern there).
 * Read here in Case 2 to report success or failure to the user.
 *   1 = key was found and deleted.
 *   0 = key was not found (or tree was empty).
 */
int status;


/**
 * @brief  Prints the styled menu banner and 9 operation options.
 *
 * Extracted into a helper to keep main() readable. Uses semantic color
 * macros from color.h so the visual theme can be changed in one place.
 * All color escapes are closed with COLOR_RESET to prevent bleed.
 */
static void print_menu(void)
{
    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);
    printf(CLR_TITLE   "      RED-BLACK TREE  -  MAIN MENU   \n" COLOR_RESET);
    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);

    const char *options[] = {
        "  1. Insert",
        "  2. Delete",
        "  3. Search",
        "  4. Display",
        "  5. Find MIN",
        "  6. Find MAX",
        "  7. Delete MIN",
        "  8. Delete MAX",
        "  9. Exit"
    };

    for (int i = 0; i < 9; i++) {
        printf(CLR_OPTION "%s\n" COLOR_RESET, options[i]);
    }

    printf(CLR_DIVIDER "======================================\n" COLOR_RESET);
    printf(CLR_PROMPT  "Enter your choice: " COLOR_RESET);
}


/**
 * @brief  Program entry point. Runs the interactive menu loop.
 *
 * The do-while loop ensures the menu is displayed at least once before
 * checking the exit condition. The loop exits cleanly on choice == 9
 * after free_tree() has released all heap memory.
 *
 * @return 0 on normal exit (the only exit path is Case 9).
 */
int main(void)
{
    node_t *root = NULL;  /* The tree starts empty. NULL is a valid empty state. */
    int choice;

    do {
        printf("\n");
        print_menu();
        scanf("%d", &choice);

        switch (choice)
        {
            /* ── Case 1: Insert ─────────────────────────────────────────── */
            case 1:
            {
                int data;
                printf(CLR_PROMPT "Enter the data: " COLOR_RESET);
                scanf("%d", &data);

                /*
                 * insert() returns SUCCESS or FAILURE.
                 * FAILURE covers two sub-cases: malloc error and duplicate key.
                 * Both are reported the same way to the user.
                 */
                if (insert(&root, data) == SUCCESS)
                    printf(CLR_SUCCESS "[INFO] : %d inserted successfully.\n"
                           COLOR_RESET, data);
                else
                    printf(CLR_ERROR "[INFO] : Insert failed "
                           "(duplicate key or memory error).\n" COLOR_RESET);
                break;
            }

            /* ── Case 2: Delete ─────────────────────────────────────────── */
            case 2:
            {
                int key;
                printf(CLR_PROMPT "Enter the data you want to delete: "
                       COLOR_RESET);
                scanf("%d", &key);

                if (root) {
                    /*
                     * delete_t() sets the global `status` and returns the
                     * (possibly new) root. We must reassign root here because
                     * deleting the root node or triggering fix-up rotations
                     * can change which node sits at the top of the tree.
                     */
                    root = delete_t(root, key);
                    if (status)
                        printf(CLR_SUCCESS "[INFO] : Element deleted "
                               "successfully.\n" COLOR_RESET);
                    else
                        printf(CLR_ERROR "[INFO] : Element not found.\n"
                               COLOR_RESET);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }

            /* ── Case 3: Search ─────────────────────────────────────────── */
            case 3:
            {
                int key;
                printf(CLR_PROMPT "Enter the data you want to search: "
                       COLOR_RESET);
                scanf("%d", &key);

                /*
                 * search_t() returns a pointer to the found node, or NULL.
                 * We only care whether the pointer is non-NULL (found) or NULL
                 * (not found); we don't use the node itself here.
                 */
                if (search_t(root, key) != NULL)
                    printf(CLR_SUCCESS "[INFO] : %d found in the tree.\n"
                           COLOR_RESET, key);
                else
                    printf(CLR_ERROR "[INFO] : %d not found.\n"
                           COLOR_RESET, key);
                break;
            }

            /* ── Case 4: Display ─────────────────────────────────────────── */
            case 4:
                printf(CLR_INFO "\n[Tree -- In-Order Traversal]\n" COLOR_RESET);
                if (root)
                    display_tree(root);  /* Prints nodes left-to-right (ascending). */
                else
                    printf(CLR_ERROR "(empty tree)\n" COLOR_RESET);
                printf("\n");
                break;

            /* ── Case 5: Find MIN ────────────────────────────────────────── */
            case 5:
            {
                node_t *min = find_min(root);
                if (min)
                    printf(CLR_SUCCESS "[INFO] : MIN = %d\n" COLOR_RESET,
                           min->data);
                else
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                break;
            }

            /* ── Case 6: Find MAX ────────────────────────────────────────── */
            case 6:
            {
                node_t *max = find_max(root);
                if (max)
                    printf(CLR_SUCCESS "[INFO] : MAX = %d\n" COLOR_RESET,
                           max->data);
                else
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                break;
            }

            /* ── Case 7: Delete MIN ──────────────────────────────────────── */
            case 7:
            {
                node_t *min = find_min(root);
                if (min) {
                    /*
                     * CRITICAL: save min->data as an integer BEFORE calling
                     * delete_t(). delete_t() calls free(z) internally, so
                     * reading min->data after deletion is use-after-free
                     * undefined behavior.
                     */
                    int val = min->data;
                    root = delete_t(root, val);
                    printf(CLR_SUCCESS "[INFO] : MIN node (%d) deleted.\n"
                           COLOR_RESET, val);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }

            /* ── Case 8: Delete MAX ──────────────────────────────────────── */
            case 8:
            {
                node_t *max = find_max(root);
                if (max) {
                    /*
                     * Same use-after-free protection as Case 7:
                     * save the integer value before the deletion frees the node.
                     */
                    int val = max->data;
                    root = delete_t(root, val);
                    printf(CLR_SUCCESS "[INFO] : MAX node (%d) deleted.\n"
                           COLOR_RESET, val);
                } else {
                    printf(CLR_ERROR "[INFO] : Tree is empty.\n" COLOR_RESET);
                }
                break;
            }

            /* ── Case 9: Exit ────────────────────────────────────────────── */
            case 9:
                /*
                 * free_tree() performs a post-order traversal and frees every
                 * node. After this call, root is a dangling pointer -- we do
                 * not set it to NULL here because the program is about to exit,
                 * but callers in other contexts should set root = NULL afterward.
                 */
                free_tree(root);
                printf(CLR_SUCCESS "\n[INFO] : Memory freed. Goodbye!\n\n"
                       COLOR_RESET);
                break;

            /* ── Default: unrecognized input ─────────────────────────────── */
            default:
                printf(CLR_ERROR "[ERROR]: Invalid option. "
                       "Please choose 1-9.\n" COLOR_RESET);
        }

    } while (choice != 9);

    return 0;
}