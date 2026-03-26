#include "tree.h"

int main()
{
    node_t *root = NULL;
    int choice;
    char *menu[] = {"1. Insert", "2. Delete", "3. Search", "4. Display", "5. Find MIN", "6. Find MAX", "7. Exit"};

    do {
        // FIX 1: Changed condition to i < 5
        for(int i = 0; i < 7; i++) 
        {
            printf("%s\n", menu[i]);
        }
        
        printf("Enter the choice: ");
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1:
            {
                int data;
                printf("Enter the data : ");
                scanf("%d", &data);
                insert(&root, data);
                break;
            }
            case 2:
                break;
            case 3:
            {
                int key;
                printf("Enter the data you want to search : ");
                scanf("%d", &key);
                if(search_t(root, key) == SUCCESS)
                    printf("Data is Found in the Tree\n");
                else
                    printf("Data is Not Found\n");
                break;
            }
            case 4:
                display_tree(root); 
                printf("\n");       
                break;
            case 5:
            {
                printf("MIN = %d\n", find_min(root));
                break;
            }
            case 6:
            {
                printf("MAX = %d\n", find_max(root));
                break;
            }
            case 7:
                break;
            default:
                printf("Invalid Option\n");
        }
    } while(choice != 5);
    
    return 0;
}