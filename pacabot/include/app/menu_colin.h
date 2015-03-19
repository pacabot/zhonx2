/*
 * menu_colin.h
 *
 *  Created on: 4 déc. 2014
 *      Author: colin
 */

#ifndef MENU_COLIN_H_
#define MENU_COLIN_H_

#define MAX_LINE_SCREEN 5
#define MAX_DOTES 300
typedef struct {
char *name;
char type;
int (*param)(void);
}lineItem;

typedef struct{
		char *name;
		lineItem line[20];
}menuItem;
typedef struct listDote
{
	int dotes [MAX_DOTES];
	struct listDote *next;
} listDote;
int menu_colin(menuItem);
void menu_animate(unsigned char y, unsigned char max_y);
void affiche_menu(menuItem menu,int first_line);
void graphMotorSettings (float *acceleration, float *maxSpeed, float *deceleration);
void printGraphMotor (float acceleration, float maxSpeed, float deceleration);

#endif /* MENU_COLIN_H_ */
