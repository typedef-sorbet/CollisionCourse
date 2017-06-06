#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct object
{
   char *name;
   char *description;
} Item;

typedef struct inventory
{
   Item items[50]; //Array of 50 item objects
   int gold;
   int numItems;
} Inventory;

