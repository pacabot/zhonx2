/*
 * resolution_maze.h
 *
 *  Created on: 27 sept. 2014
 *      Author: zhoe
 */

#ifndef RESOLUTION_MAZE_H_
#define RESOLUTION_MAZE_H_
#define MAZE_SIZE 16
//#define true 1
//#define false 0
//#define NULL 0

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define FORWARD 0
#define RIGHT 1
#define UTURN 2
#define LEFT 3
#define NO_KNOW 0
#define WALL_KNOW 1
#define NO_WALL 2
//#define test

#include <stdlib.h>

//Structures typedef
typedef struct
{
  char wall_north;
  char wall_south;
  char wall_east;
  char wall_west;
  int length;
}one_cell;

typedef struct
{
  one_cell cell[MAZE_SIZE][MAZE_SIZE];
}labyrinthe;

typedef struct
{
  int right;
  int front;
  int left;
}inputs;

typedef struct coordinate
{
  int x;
  int y;
  struct coordinate *next;
  struct coordinate *previous;
}coordinate;
typedef struct
{
    char x;
    char y;
    char orientation;
    bool midOfCase;
} positionRobot;


// fonctions
extern int mazeColin(void);
void exploration(labyrinthe *maze, positionRobot* poitionZhonx,char xFinish, char yFinish);
void moveVirtualZhonx(labyrinthe maze, positionRobot positionZhonxVirtuel,coordinate *way, char xFinish, char yFinish);
void move_zhonx (int direction_to_go, char *direction_robot, int numberOfCase);
void new_cell(inputs new_walls, labyrinthe *maze, positionRobot positionZhonx);
void new_dot(coordinate **old_dot,int x,int y);
void poids(labyrinthe *maze, int xFinish, int yfinish, bool wallNoKnow);
void maze_init (labyrinthe *maze);
void* calloc_s (size_t nombre, size_t taille);
void print_maze(const labyrinthe maze, const int x_robot, const int y_robot);
void print_length(const labyrinthe maze);
void clearMazelength(labyrinthe* maze);
inputs see_walls ();
void commande(int dir, int dist);
bool mini_way_find(labyrinthe *maze,char xStart, char yStart, char xFinish, char yFinish);
void trajectoire1(char *Tab, int taille);
void moveRealZhonx(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way, char *endX, char *endY);
void run (labyrinthe *maze,positionRobot *positionZhonx, char xFinish, char yFinish);
void moveRealZhonxArc(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way);
void move_zhonx_arc (int direction_to_go, positionRobot *positionZhonx, int numberOfCase, bool endMidOfCase);
void testMoveRealZhonx(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way, char *endX, char *endY);
void doUTurn(positionRobot *positionZhonx);
void waitStart(void);
void goOrientation(char *orientationZhonx, char directionToGo);
void calibrateSimple();
bool diffWay(coordinate *way1,coordinate *way2);
void deletWay(coordinate *way);
#endif /* RESOLUTION_MAZE_H_ */
