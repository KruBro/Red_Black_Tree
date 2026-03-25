#include "tree.h"
// Added for safety

int main()
{
    node_t *root = NULL;
    int choice;
    char *menu[] = {"1. Insert", "2. Delete", "3. Search", "4. Display", "5. Exit"};

    do {
        // FIX 1: Changed condition to i < 5
        for(int i = 0; i < 5; i++) 
        {
            printf("%s\n", menu[i]);
        }
        
        printf("Enter the choice: ");
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1: { // FIX 3: Added block braces for variable declaration
                int data;
                printf("Enter the data : ");
                scanf("%d", &data);
                insert(&root, data);
                break;
            }
            case 2:
                break;
            case 3:
                break;
            case 4:
                display_tree(root); // FIX 2: Corrected function name
                printf("\n");       // Added newline for cleaner output
                break;
            case 5:
                break;
            default:
                printf("Invalid Option\n");
        }
    } while(choice != 5);
    
    return 0;
}