#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "assets.h"




void messHall();
void bedroom();
void controlRoom();
void hatch();
void engine();
void controlPanel();
void electrical();
void cargo();
void wirePanel();

int checkForItem(char *);
void removeItem(char*);
void drawInv();
void drawMap(char *, char *, char *, char *, char *, char *);
char getCardinalDirection(char *str);
void addItem(char*, char*);
void examineItem(char*);
char* toLowerCase(char*);

int max_x, max_y;
int display_y, display_x, text_y, text_x, map_y, map_x, inv_y, inv_x;

//a BUNCH of gamestate flags
bool wireFirst = TRUE, cargoFirst = TRUE, engineFirst = TRUE, electricalFirst = TRUE, bedroomFirst = TRUE, controlFirst = TRUE, hatchFirst = TRUE, messHallFirst = TRUE, controlPanelFirst = TRUE;
bool wirePanelFixed = FALSE, engineWorking = FALSE;
bool engineHatchLocked = TRUE;
bool firstWirePair = FALSE, secondWirePair = FALSE, thirdWirePair = FALSE, fourthWirePair = FALSE; 
int batteryCharge = 0;

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
	start_color();
	
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	
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
		wprintw(display, "\nA nightstand sits next to the bed pod, only inches away from the pillow.\n");
		if(checkForItem("Slippers") == -1)
			wprintw(display, "Your comfy slippers sit at the edge of the bed, on a small rug.");
		wprintw(display, "\nTo the south is the control room. A ladder leads upwards to a debug message.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
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
					if(checkForItem("Cup") == -1)
						wprintw(display, "\nAn empty plastic cup sits atop the metal nightstand.\n");
					else
						wprintw(display, "\nThe metal nightstand sits alone and empty next to the bed.\n");
				}
				if(strstr(command, "cup") != NULL && checkForItem("Cup") == -1)
				{
					wprintw(display, "\nGotten.\n");
					addItem("Cup", "A small plastic cup. Good for holding things.");
				}
				if(strstr(command, "slippers") != NULL && checkForItem("Slippers") == -1)
				{
					wprintw(display, "\nYou don the pink and fuzzy slippers, basking in true comfort.\n");
					addItem("Slippers", "The pinnacle of luxury footwear.");
				}
				if(strstr(command, "rug") != NULL && checkForItem("Slippers") != -1)
				{
					if(checkForItem("Static") == -1)
					{
						wprintw(display, "\nAs you walk on the rug, you hear a faint clicking noise as static\nbegins to build up on your body.\n");
						addItem("Static", "Who knew shuffling around on a rug could become so electric?");
					}
					else
						wprintw(display, "\nYou've already built up enough static. Your hair's standing on end right now.\n");
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
		if(!wirePanelFixed)
		{
			wprintw(display, "\nMost of the control panels in this room seem to be working, except for the\nmain control panel that activates the nav system. How inconvenient.\n");
		}
		else
		{
			wprintw(display, "\nThe main control panel happily blinks away, showing the current status of the ship.\n");
		}
		wprintw(display, "\nDoors lead out to the mess hall to the east, and the bedrooms to the north.\nA maintenance hatch is on the floor of the room, near the captain's chair.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
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
				if(strstr(command, "panel") != NULL && wirePanelFixed)
				{
					if(checkForItem("Access Code") != -1)
						controlPanel();
					else
						wprintw(display, "\nThe control panel requires an access code.\n");
				}
				break;
		}
		
		wrefresh(display);
	}
}

void controlPanel()
{
	wprintw(display, "\n");
	drawMap(NULL, NULL, NULL, NULL, NULL, NULL);
	if(controlPanelFirst)
	{
		wprintw(display, "\nYou type \"hunter2\" on the on screen keyboard, and a large \"ACCESS GRANTED\" message appears.\nYou're in.\n");
		controlPanelFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nMAIN SHIP CONTROLS\nPlease select an action to take.\nA) Activate Main Thrusters\nB) Activate Self Destruct Sequence (for emergencies only!)\nC) Exit");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		if(command[0] == 'A' || command[0] == 'a')
		{
			if(batteryCharge == 3 && engineWorking)
			{
				wprintw(display, "\nYou activate the main thrusters of the ship, and in the nick of time, the ship\ncorrects its course away from the asteroid into safety.\nYou can go back to sleep now. You've earned it. You're safe now.\n\nGOOD END\n");
				wrefresh(display);
				sleep(3);
				endwin();
				exit(0);
			}
			else
			{
				if(batteryCharge < 3)
					wprintw(display, "\nError: Engine power is at suboptimal levels; thrusters cannot be activated.\n");
				if(!engineWorking)
					wprintw(display, "\nError: Engine currently nonfunctional. Notify maintenance crew immediately.\nWould you like to send an error report?\n");
			}
		}
		else if(command[0] == 'B' || command[0] == 'b')
		{
			wprintw(display, "\nJust know that you did this to yourself.\n\nBAD END\n");
			wrefresh(display);
			sleep(3);
			endwin();
			exit(0);
		}
		else if(command[0] == 'C' || command[0] == 'c')
		{
			controlRoom();
		}

		wrefresh(display);
	}
}

void wirePanel()
{
	wprintw(display, "\n");
	drawMap(NULL, NULL, NULL, NULL, NULL, NULL);
	if(wireFirst)
	{
		wprintw(display, "\nWhen you pry off the panel covering the wires, you're greeted by two side panels\n that were once connected by insulated copper. Each side, left and right, has color coded wires.\nMany wires are disconnected from one another.\n");
		wireFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nThe wires are still not fully connected.\nUse the syntax \"C to A\" to connect the third wire on the left to the first wire on\nthe right, for example.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				break;
			case 's':
				break;
			case 'e':
				break;
			case 'w':
				break;
			case 'u':
				break;
			case 'd':
				break;
			case 'q':
				endwin();
				exit(0);
			default:		//additional checks
				if(strstr(command, "exit") != NULL)
					electrical();
				if(strstr(command, "look") != NULL && strstr(command, "left") != NULL)
					wprintw(display, "\nFrom top to bottom, the color codes are red, yellow, blue, green.\n");
				if(strstr(command, "look") != NULL && strstr(command, "right") != NULL)
					wprintw(display, "\nFrom top to bottom, the color codes are yellow, blue, red, green.\n");
				if((command[0] == 'a' || command[0] == 'A') && (command[5] == 'c' || command[5] == 'C'))
				{
					wprintw(display, "\nConnected successfully.\n");
					firstWirePair = TRUE;
				}
				else if((command[0] == 'b' || command[0] == 'B') && (command[5] == 'a' || command[5] == 'A'))
				{
					wprintw(display, "\nConnected successfully.\n");
					secondWirePair = TRUE;
				}
				else if((command[0] == 'c' || command[0] == 'C') && (command[5] == 'b' || command[5] == 'B'))
				{
					wprintw(display, "\nConnected successfully.\n");
					thirdWirePair = TRUE;
				}
				else if((command[0] == 'd' || command[0] == 'D') && (command[5] == 'd' || command[5] == 'D'))
				{
					wprintw(display, "\nConnected successfully.\n");
					fourthWirePair = TRUE;
				}
				else if(command[2] == 't' && command[3] == 'o')
					wprintw(display, "\nThis is not a valid connection. Try again.\n");
				if(firstWirePair && secondWirePair && thirdWirePair && fourthWirePair)
				{
						wirePanelFixed = TRUE;
						wprintw(display, "\nThe wire panel is now fixed and operational. Nice job.\n");
						electrical();
				}
				break;
		}
		wrefresh(display);
	}
}

void hatch()
{
	wprintw(display, "\n");
	drawMap("Cargo Bay", NULL, NULL, NULL, "Control Room", NULL);
	if(hatchFirst)
	{
		wprintw(display, "The dimly lit corridor of the maintenance hatch is lined with steel pipes,\ndesigned to carry the blood of the ship.\nIn less literary terms, you're next to the fuel lines.\nInterestingly enough, someone seems to have scrawled some writing on the pipes.\n");
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
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				cargo();
				break;
			case 'u':
				controlRoom();	
				break;
			case 'q':
				endwin();
				exit(0);
			default:		
				if(strstr(command, "look") != NULL && (strstr(command, "writing") != NULL || strstr(command, "pipe") != NULL) && checkForItem("Access Code") == -1)
				{
					wprintw(display, "\nAs you examine the writing on the pipes, you can faintly make out a string\nof characters. \"hunter2\". Perhaps this is a code?\nYou take note of this writng.\n");
					addItem("Access Code" , "A code that might come in handy later. \"hunter2\".");
				}
				break;
		}
		
		wrefresh(display);
	}
}

void cargo()
{
	wprintw(display, "\n");
	drawMap(NULL, "Fuel Lines", "Engine Room", NULL, NULL, NULL);
	if(cargoFirst)
	{
		wprintw(display, "\nBoxes, boxes, and more boxes. Toolboxes, wooden boxes, all kinds of boxes.\n");
		cargoFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "A toolbox lies open atop a waist-high crate.\nTo the south are the fuel lines and the maintenance hatch, and to\nthe east is the engine room.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				break;
			case 's':
				hatch();
				break;
			case 'e':
				engine();
				break;
			case 'w':
				break;
			case 'u':
				break;
			case 'd':
				break;
			case 'q':
				endwin();
				exit(0);
			default:		//additional checks
				if(strstr(command, "look") != NULL && strstr(command, "toolbox") != NULL)
					wprintw(display, "\nInside the toolbox, you find a wrench amongst an array of broken tools.\n");
				if(strstr(command, "wrench") != NULL && checkForItem("Wrench") == -1)
				{
					wprintw(display, "\nPicked up the wrench.\n");
					addItem("Wrench", "It's like a wrench, but more...space-y.");
				}
				break;
		}
		wrefresh(display);
	}
}

void engine()
{
	wprintw(display, "\n");
	drawMap(NULL, NULL, NULL, "Cargo Bay", "Electrical Room", NULL);
	if(engineFirst)
	{
		wprintw(display, "\nAs you enter this room, you hear the familiar hum of the engine hard\nat work...Wait, no you don't. That might be a problem.\n");
		engineFirst = FALSE;
	}
	while(1)
	{
		if(engineWorking)
		{
			wprintw(display, "\nThe engine on the far wall spins its gears happily, supplying power to the thrusters.\n");
		}
		else
		{
			wprintw(display, "\nThe engine on the far wall groans painfully, its gears seemingly stuck.\n");
		}
		wprintw(display, "/nTo the west is the cargo bay. Above you, there is a hatch to the electrical room./n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				break;
			case 's':
				break;
			case 'e':
				break;
			case 'w':
				cargo();
				break;
			case 'u':
				if(engineHatchLocked)
					wprintw(display, "\nThe hatch appears to be locked from the other side.\n");
				else
					electrical();
				break;
			case 'd':
				break;
			case 'q':
				endwin();
				exit(0);
			default:		//additional checks
				if(strstr(command, "look") != NULL && strstr(command, "engine") != NULL)
				{
					if(engineWorking)
					{
						wprintw(display, "\nThe engine is working happily to make the ship work.\nLet's keep it that way.\n");
					}
					else
						wprintw(display, "\nThe gears of the engine are stuck, and appear to not be able to move past\none another. Perhaps some lubricant will fix the problem.\n");
				}
				if((strstr(command, "engine") != NULL || strstr(command, "gear") != NULL) && (strstr(command, "slim") != NULL || strstr(command, "lub") != NULL || strstr(command, "food") != NULL))
				{
					if(!engineWorking && checkForItem("Cup w/ Slime") != -1)
					{
						wprintw(display, "\nYou pour the slime onto the gears, allowing it to seep into every\nnook and cranny. The gears begin to croak to life, spinning\nhappily and getting the ship back on its metaphorical feet.\n");
						removeItem("Cup w/ Slime");
						addItem("Cup", "A small plastic cup. Good for holding things.");
					}
					else
						wprintw(display, "\nYou don't need to mess with the engine anymore.\n");
				}
				break;
		}
		wrefresh(display);
	}
}

void messHall()
{
	wprintw(display, "\n");
	drawMap("Electrical Room", NULL, NULL, "Control Room", NULL, NULL);
	if(messHallFirst)
	{
		wprintw(display, "\nThe mess hall is a complete mess. How apt.\n");
		messHallFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nIn the corner sits a machine labeled \"Food\". To the north is the electrical room, and to the\nwest is the control room.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				electrical();
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

void electrical()
{
	wprintw(display, "\n");
	drawMap(NULL, "Mess Hall", NULL, NULL, NULL, "Engine Room");
	if(electricalFirst)
	{
		wprintw(display, "\nI would describe this room to you in detail, but I'm a computer science major.\nIf you want a detailed description of the electrical workings of a spaceship, you\nshould play a game made by an electrical engineer, not me.\n");
		electricalFirst = FALSE;
	}
	while(1)
	{
		wprintw(display, "\nA large battery sits in the middle of the room, with an exposed metal panel and a\nsign labeled \"Engine Power\". On the wall behind the battery, a panel is loosely\nbolted to the wall, covering some wires.\nA locked hatch leads downwards to the engine room, and the mess hall is to the south.\n");
		wrefresh(display);
		wmove(textWindow, 1, 1);
		wgetnstr(textWindow, command, 30);
		wclear(textWindow);
		wborder(textWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		wrefresh(textWindow);
		
		attron(COLOR_PAIR(1));
		wprintw(display, "\n\t%s\n", command);
		attroff(COLOR_PAIR(1));
		
		char token = getCardinalDirection(command);
		switch(token)
		{
			case 'n':
				break;
			case 's':
				messHall();
				break;
			case 'e':
				break;
			case 'w':
				break;
			case 'u':
				break;
			case 'd':
				if(checkForItem("Silver Key") != -1)
				{
					if(engineHatchLocked)
					{
						wprintw(display, "\nYou use the silver key on the locked hatch, and it fits!");
						engineHatchLocked = FALSE;
					}
					//engine();
				}
				break;
			case 'q':
				endwin();
				exit(0);
			default:		//additional checks
				if(strstr(command, "hatch") != NULL)
				{
					if(checkForItem("Silver Key") != -1)
					{
						if(engineHatchLocked)
						{
							wprintw(display, "\nYou use the silver key on the locked hatch, and it fits!");
							engineHatchLocked = FALSE;
						}
						engine();
					}
				}
				if(strstr(command, "panel") != NULL)
				{
					if(checkForItem("Wrench") != -1)
					{
						if(!wirePanelFixed)
							wirePanel();
						else
							wprintw(display, "\nThe panel is fixed, you don't need to mess with it anymore.\n");
					}
					else
					{
						wprintw(display, "\nYou don't seem to have a way of removing the panel cover.\nAnd before you ask: no, you're not strong enough to rip it off.\n");
					}
				}
				if(strstr(command, "look") != NULL && strstr(command, "battery") != NULL)
				{
					wprintw(display, "\nThe battery has an exposed metal panel, which appears to be connected to the\narea where the charge is stored.\n");
				}
				if(strstr(command, "touch") != NULL && (strstr(command, "battery") != NULL))
				{
					if(batteryCharge < 3)
					{
						if(checkForItem("Static") != -1)
						{
							++batteryCharge;
							removeItem("Static");
							if(batteryCharge == 1)
								wprintw(display, "\nAn indicator on the battery shows that it is one third charged.\n");
							else if(batteryCharge == 2)
								wprintw(display, "\nThe indicator now shows that the battery is two thirds charged.\n");
							else
								wprintw(display, "\nThe battery is now fully charged.\n");
						}
					}
					else
						wprintw(display, "\nThe battery is already charged.\n");
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
	drawInv();
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