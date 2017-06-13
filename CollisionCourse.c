#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "assets.h"


void drawInv();
void drawMap(char *, char *, char *, char *, char *, char *);
int checkForItem(char *);
void removeItem(char*);
char getCardinalDirection(char *str);
void bedroom();
void controlRoom();
void hatch();
void addItem(char*, char*);
void examineItem(char*);
char* toLowerCase(char*);
void messHall();

int max_x, max_y;
int display_y, display_x, text_y, text_x, map_y, map_x, inv_y, inv_x;

//a BUNCH of gamestate flags
bool bedroomFirst = TRUE, controlFirst = TRUE, hatchFirst = TRUE, messHallFirst = TRUE;

//buffer for commands and text
char command[30];
	

Inventory inv = {{}, 0};

WINDOW *textWindow;
WINDOW *display;
WINDOW *invWin;
WINDOW *mapWin;

int main()
{
	initscr();
	echo();
	curs_set(FALSE);
	cbreak();
	
	int enterSize = 3;
	int otherWidth = 25;
	int mapSize = 8;

	getmaxyx(stdscr, max_y, max_x);
	
	//initialize each window
	textWindow = newwin(enterSize, max_x-otherWidth, max_y-enterSize, 0);
	display = newwin(max_y - enterSize, max_x-otherWidth, 0, 0);
	invWin = newwin(max_y - mapSize, otherWidth, 0, max_x-otherWidth);
	mapWin = newwin(mapSize, otherWidth, max_y-mapSize, max_x-otherWidth);
	
	//get maximum coords for each window
	getmaxyx(display, display_y, display_x);
	getmaxyx(textWindow, text_y, text_x);
	getmaxyx(display, map_y, map_x);
	getmaxyx(textWindow, inv_y, inv_x);
	
	//let flavor text scroll
	scrollok(display, TRUE);
	idlok(display, TRUE);
	
	//set borders for all except display
	wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(mapWin, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(invWin, '|', '|', '-', '-', '+', '+', '+', '+');
	
	
	wrefresh(invWin);
	wrefresh(mapWin);
	
	//move cursors for each win
	wmove(display, display_y-2, 0);
	wmove(invWin, 1, 1);
	wmove(mapWin, 1, 1);
	wmove(textWindow, 1, 1);
	
	drawInv();
	
	bedroom();
	
	/*while(1)
	{
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, str, 30);
		drawInv();
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wprintw(display, "%s\n", str);
		wrefresh(display);
		wrefresh(textWindow);
	}*/

	/*	Basic window workflow:
		-move cursor to (1,1) in textWindow
		-wgetnstr
		-clear textWindow
		-redraw textWindow's borders
		-refresh textWindow
		-react to input
		-refresh display
	*/
}

void bedroom()
{
	wprintw(display, "\n");
	drawMap(NULL, "Control Room", NULL, NULL, "Debug Message" , NULL);
	if(bedroomFirst)
	{
		wprintw(display, "You awaken with a start under the covers of your bed.\nThis would not be unusual, if it weren't for the fact that there was a large\n\"COLLISION COURSE ALERT\" alarm blaring in the background.\nOh well. there was something horribly uncomfortable about the pillow anyways.\n");
		bedroomFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nA nightstand sits next to the bed pod, only inches away from the pillow.\nTo the south is the control room. A ladder leads upwards to a debug message.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 's':
				controlRoom();
				break;
			case 'u':
				wprintw(display, "\nLadders aren't good for interior decoration, you know.\n");
				break;
			case 'q':
				endwin();
				exit(0);
			default:
				if(strstr(command, "go back to bed") != NULL || strstr(command, "sleep") != NULL)
				{
					wprintw(display, "Guess you'll just be crashing then. Sweet dreams.\n");
					wprintw(display, "\n\tSLEEP ENDING\n");
					wrefresh(display);
					sleep(3);
					endwin();
					exit(0);
				}
				if(strstr(command, "ladder") != NULL)
					wprintw(display, "\nLadders aren't good for interior decoration, you know.\n");
				if(strstr(command, "look") != NULL && strstr(command, "pillow") != NULL)
				{
					if(checkForItem("Silver Key") == -1)
					{
						wprintw(display, "\nYou find a silvery key hiding underneath the pillow.\n");
						addItem("Silver Key", "A key with a silver finish. Found under your pillow.");
					}
					else
						wprintw(display, "\nYou look under the pillow once more, but only find lint and disappointment.\n");
				}
				if(strstr(command, "look") != NULL && strstr(command, "nightstand") != NULL)
				{
					wprintw(display, "\nAn empty plastic cup sits atop the metal nightstand.\n");
				}
				if(strstr(command, "cup") != NULL && checkForItem("Cup") == -1)
				{
					wprintw(display, "\nGotten.\n");
					addItem("Cup", "A small plastic cup. Good for holding things.");
				}
				break;
		}
		wrefresh(display);
	}
}

void controlRoom()
{
	wprintw(display, "\n");
	drawMap("Bedrooms", NULL, "Mess Hall", NULL, NULL, "Hatch");
	if(controlFirst)
	{
		wprintw(display, "A myriad of bleeps, bloops, and flashing lights flood your sleep-deprived eyes.\nLuckily, you know what all of them mean.\nWell, you, the character. Not you, necessarily. I mean, did you pass intergalactic flight school?\n");
		controlFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nDoors lead out to the mess hall to the east, and the bedrooms to the north.\nA maintenance hatch is on the floor of the room, near the captain's chair.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'e':
				messHall();
				break;
			case 'n':
				bedroom();
				break;
			case 'd':
				hatch();				
				break;
			case 'q':
				endwin();
				exit(0);
			default:
				if(strstr(command, "hatch") != NULL)
					hatch();		
				break;
		}
		
		wrefresh(display);
	}
}

void hatch()
{
	wprintw(display, "\n");
	drawMap("Hatch Part II", NULL, NULL, NULL, "Control Room", NULL);
	if(hatchFirst)
	{
		wprintw(display, "The dimly lit corridor of the maintenance hatch is lined with steel pipes,\ndesigned to carry the blood of the ship.\nIn less literary terms, you're next to the fuel lines.\n");
		hatchFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "The hatch continues on to the north following the fuel lines. The exit to the hatch is directly above you.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				wprintw(display, "\nThats more of the hatch.\n");
				break;
			case 'u':
				controlRoom();	
				break;
			case 'q':
				endwin();
				exit(0);
			default:		
				break;
		}
		
		wrefresh(display);
	}
}

void messHall()
{
	wprintw(display, "\n");
	drawMap("Engineering Room", NULL, NULL, "Control Room", NULL, NULL);
	if(messHallFirst)
	{
		wprintw(display, "\nThe mess hall is a complete mess. How apt.\n");
		messHallFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nIn the corner sits a machine labeled \"Food\". To the north is the engineering room, and to the\nwest is the control room.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				//engineering();
				break;
			case 's':
				break;
			case 'e':
				break;
			case 'w':
				controlRoom();
				break;
			case 'u':
				break;
			case 'd':
				break;
			case 'q':
				endwin();
				exit(0);
			default:		//additional checks
				if(strstr(command, "machine") != NULL)
				{
					int index;
					if((index = checkForItem("Cup")) != -1)
					{
						if(checkForItem("Cup w/ Slime") == -1)
						{
							wprintw(display, "\nYou put some slimy food into the cup.\n");
							removeItem("Cup");
							addItem("Cup w/ Slime", "A plastic cup containing a slimy substance described as food.");
						}
					}
					else if(checkForItem("Cup w/ Slime") != -1)
						wprintw(display, "\nWhat, you want to carry around MORE slime?\n");
					else
						wprintw(display, "\nYou pull the lever on the machine, and slimy, gross goop begins to pour on the ground.\n");
				}
				break;
		}
		wrefresh(display);
	}
}

void drawInv()
{
	mvwprintw(invWin, 1, 1, "Inventory");
	wmove(invWin, 3, 1);
	int invCursor = 3;
	while(invCursor <= inv.numItems+2)
	{
			wprintw(invWin, inv.items[invCursor-3].name);
			wmove(invWin, ++invCursor, 1);
	}
	wrefresh(invWin);
}

void removeItem(char *itemName)
{
	int indexOfItem = checkForItem(itemName);
	if(indexOfItem == -1)
		return;
	for(indexOfItem; indexOfItem < inv.numItems; indexOfItem++)
	{
			inv.items[indexOfItem] = inv.items[indexOfItem+1];
	}
	inv.numItems--;
}

//should the indexes be hardcoded? no.
//am I going to do it anyways? yeah.
void drawMap(char *north, char *south, char *east, char *west, char *up, char *down)
{
	wclear(mapWin);
	wborder(mapWin, '|', '|', '-', '-', '+', '+', '+', '+');
	wmove(mapWin, 1, 1);
	wprintw(mapWin, "North: ");
	if(north != NULL)
		wprintw(mapWin, "%s", north);
	wmove(mapWin, 2, 1);
	wprintw(mapWin, "South: ");
	if(south != NULL)	
		wprintw(mapWin, "%s", south);
	wmove(mapWin, 3, 1);
	wprintw(mapWin, "East: ");
	if(east != NULL)	
		wprintw(mapWin, "%s", east);
	wmove(mapWin, 4, 1);
	wprintw(mapWin, "West: ");
	if(west != NULL)	
		wprintw(mapWin, "%s", west);
	wmove(mapWin, 5, 1);
	wprintw(mapWin, "Up: ");
	if(up != NULL)	
		wprintw(mapWin, "%s", up);
	wmove(mapWin, 6, 1);
	wprintw(mapWin, "Down: ");
	if(down != NULL)	
		wprintw(mapWin, "%s", down);
	wrefresh(mapWin);
}

void addItem(char *name, char* description)
{
	Item newItem;
	newItem.name = name;
	newItem.description = description;
	inv.items[inv.numItems++] = newItem;
	drawInv();
}

int checkForItem(char *find)
{
	if(find == NULL)	
		return -1;
	int i = 0;
	for(i = 0; i < inv.numItems; i++)
	{
		if(inv.items[i].name != (char *)0){
			if(strcmp(toLowerCase(inv.items[i].name), toLowerCase(find)) == 0){
				return i;
			}
		}
	}
	return -1;
}

char* toLowerCase(char* in)
{
	char *out = malloc(strlen(in)+1);
	for(int i = 0; i < strlen(in); i++)
		out[i] = tolower(in[i]);
	out[strlen(in)] = '\0';
	return out;
}

char getCardinalDirection(char *str)
{
	if(strstr(str, "south") != NULL || ((str[0] == 's') && (str[1] == '\0')))
		return 's';
	if(strstr(str, "north") != NULL || ((str[0] == 'n') && (str[1] == '\0'))) 
		return 'n';
	if(strstr(str, "east") != NULL || ((str[0] == 'e') && (str[1] == '\0')))
		return 'e';
	if(strstr(str, "west") != NULL || ((str[0] == 'w') && (str[1] == '\0')))
		return 'w';
	if((strstr(str, "up") != NULL && strstr(str, "cup") == NULL) || ((str[0] == 'u') && (str[1] == '\0')))
		return 'u';
	if(strstr(str, "down") != NULL || ((str[0] == 'd') && (str[1] == '\0')))
		return 'd';
	if(strstr(str, "quit") != NULL)
		return 'q';
	if(strstr(str, " examine ") != NULL)
		examineItem(str);
	return 'z';	
}

void examineItem(char *command)
{
		char *itemToFind = &command[8];	//this will totally always work
		int indexInInv = checkForItem(itemToFind);
		if(indexInInv == -1)
			return;
		wprintw(display, "\n%s\n", inv.items[indexInInv].description);
		wrefresh(display);
}