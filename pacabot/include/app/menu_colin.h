/*
 * menu_colin.h
 *
 *  Created on: 4 déc. 2014
 *      Author: colin
 */

#ifndef MENU_COLIN_H_
#define MENU_COLIN_H_

#define MAX_LINE_SCREEN 5
typedef struct {
char *name;
char type;
int (*param)(void);
}lineItem;

typedef struct{
		char *name;
		lineItem line[20];
}menuItem;
int menu_colin(menuItem);
void menu_animate(unsigned char y, unsigned char max_y);
void affiche_menu(menuItem menu,int first_line);
void graphMotorSettings (unsigned long *acceleration, unsigned long *maxSpeed, unsigned long *deceleration);
void printGraphMotor (unsigned long acceleration, unsigned long maxSpeed, unsigned long deceleration);

#endif /* MENU_COLIN_H_ */
