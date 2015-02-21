#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx.h"

#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"
#include "stm32f4xx_gpio.h"

#include "hal/hal_os.h"
#include "hal/hal_adc.h"
#include "hal/hal_beeper.h"
#include "hal/hal_led.h"
#include "hal/hal_nvm.h"
#include "hal/hal_pid.h"
#include "hal/hal_step_motor.h"
#include "hal/hal_sensor.h"
#include "hal/hal_serial.h"
#include "hal/hal_ui.h"

#include "drivers/step_motor_driver.h"

#include "util/common_functions.h"

#include "app/app_def.h"
#include "app/menus.h"
#include "app/solver_maze.h"

#include "stm32f4xx.h"

extern int mazeColin(void)
{
	char choice=-1;
	char posXStart;
	char posYStart;
	labyrinthe maze;
	maze_init(&maze);
	positionRobot positionZhonx;
	if (zhonx_settings.color_sensor_enabled==true)
	{
		positionZhonx.x=MAZE_SIZE/2;
		positionZhonx.y=MAZE_SIZE/2;
		positionZhonx.orientation=NORTH;
		zhonx_settings.x_finish_maze=0;
		zhonx_settings.y_finish_maze=0;
	}
	else
	{
		positionZhonx.x=0;
		positionZhonx.y=0;
		positionZhonx.orientation=EAST;
	}
	positionZhonx.midOfCase=true;
	posXStart=positionZhonx.x;
	posYStart=positionZhonx.y;
	print_maze(maze,positionZhonx.x,positionZhonx.y);
	if (zhonx_settings.calibration_enabled==true)
	{
		hal_os_sleep(1000);
		calibrateSimple();
	}
	int lengthMini=-1;
	do
	{
		hal_os_sleep(2000);
		waitStart();
		exploration(&maze, &positionZhonx,zhonx_settings.x_finish_maze,zhonx_settings.y_finish_maze);
		if (zhonx_settings.calibration_enabled==true)
				calibrateSimple();
		hal_os_sleep(2000);
		exploration(&maze, &positionZhonx,posXStart,posYStart);
		if (zhonx_settings.calibration_enabled==true)
						calibrateSimple();
		doUTurn(&positionZhonx);
		} while(mini_way_find(&maze,posXStart,posYStart, zhonx_settings.x_finish_maze, zhonx_settings.y_finish_maze));
	do
	{
		choice=-1;
		waitStart();
		exploration(&maze, &positionZhonx,zhonx_settings.x_finish_maze,zhonx_settings.y_finish_maze);
		if (zhonx_settings.calibration_enabled==true)
				calibrateSimple();
		hal_os_sleep(2000);
		exploration(&maze, &positionZhonx,posXStart,posYStart);
		if (zhonx_settings.calibration_enabled==true)
				calibrateSimple();
		doUTurn(&positionZhonx);
		hal_ui_clear_scr(app_context.ui);
		hal_ui_display_txt(app_context.ui,10,10,"presse \"RIGHT\" to "); hal_ui_display_txt(app_context.ui,10,18,"do a new run");
		hal_ui_refresh(app_context.ui);
		while(choice==-1)
		{
			 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET)
			{
				// Wait until button is released
				while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET);
				hal_os_sleep(200);
				choice=1;
			}

			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET)
			{
				// Wait until button is released
				while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET);
				hal_os_sleep(200);
				choice=0;
			}
		}
	}while (choice==1);
	return HAL_UI_E_SUCCESS;
}

void exploration(labyrinthe *maze, positionRobot* positionZhonx,char xFinish, char yFinish)
{
	coordinate way={0,0,0};
	hal_step_motor_enable();
	new_cell(see_walls(),maze,*positionZhonx);

	while(positionZhonx->x!=xFinish || positionZhonx->y!=yFinish)
	{
		clearMazelength(maze);
		poids(maze,xFinish, yFinish,true);
		moveVirtualZhonx(*maze,*positionZhonx,&way,xFinish, yFinish);
	moveRealZhonx(maze,positionZhonx,way.next,&xFinish,&yFinish);
	}

	hal_os_sleep(200);
	//step_motors_move(CELL_LENGTH/2, 0, 0);
	hal_step_motor_disable();
}

void moveVirtualZhonx(labyrinthe maze, positionRobot positionZhonxVirtuel,coordinate *way, char xFinish, char yFinish)
{
	while(positionZhonxVirtuel.x!=xFinish || positionZhonxVirtuel.y!=yFinish)
			{
			clearMazelength(&maze);
			poids(&maze,xFinish, yFinish,true);
			print_maze(maze,positionZhonxVirtuel.x,positionZhonxVirtuel.y);
			if(maze.cell[positionZhonxVirtuel.x+1][positionZhonxVirtuel.y].length+1 == maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].length && positionZhonxVirtuel.x+1<MAZE_SIZE && maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].wall_east==NO_WALL)
				{
					positionZhonxVirtuel.x=positionZhonxVirtuel.x+1;
				}
			else if(maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y+1].length+1 == maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].length && positionZhonxVirtuel.y+1<MAZE_SIZE && maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].wall_south==NO_WALL)
				{
					positionZhonxVirtuel.y=positionZhonxVirtuel.y+1;
				}
			else if(maze.cell[positionZhonxVirtuel.x-1][positionZhonxVirtuel.y].length+1 == maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].length && positionZhonxVirtuel.x>0 && maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].wall_west==NO_WALL)
				{
					positionZhonxVirtuel.x=positionZhonxVirtuel.x-1;
				}
			else if(maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y-1].length+1 == maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].length && positionZhonxVirtuel.y>0 && maze.cell[positionZhonxVirtuel.x][positionZhonxVirtuel.y].wall_north==NO_WALL)
				{
					positionZhonxVirtuel.y=positionZhonxVirtuel.y-1;
				}
			else
				{
					if(way->previous!=null)
						return;
					else
					{
						int boucle=true;
						hal_ui_clear_scr(app_context.ui);
						hal_ui_display_txt(app_context.ui,0,0,"no solution");
						hal_ui_refresh(app_context.ui);
						hal_step_motor_disable();
						while (boucle)
						{
						}
					}
				}
			new_dot(&way,positionZhonxVirtuel.x,positionZhonxVirtuel.y);
			}
	return;
}

void moveRealZhonx(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way, char *endX, char *endY)
{
	coordinate *oldDote;
	int length;
	char additionY=0;
	char additionX=0;
	char orientaionToGo=NORTH;
	while(way!=null)
	{
		length=0;
		if(way->x==(positionZhonx->x+1) && way->y==positionZhonx->y)
		{
			additionX=1;
			additionY=0;
			orientaionToGo=EAST;
		}
		else if(way->x==(positionZhonx->x-1) && way->y==positionZhonx->y)
		{
			additionX=-1;
			additionY=0;
			orientaionToGo=WEST;
		}
		else if(way->y==(positionZhonx->y-1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=-1;
			orientaionToGo=NORTH;
		}
		else if(way->y==(positionZhonx->y+1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=1;
			orientaionToGo=SOUTH;
		}
		else
		{
			hal_os_sleep(200);
			hal_step_motor_disable();
			hal_ui_clear_scr(app_context.ui);
			hal_ui_display_txt(app_context.ui,0,0,"Error way");
			hal_ui_refresh(app_context.ui);
			while(1);
		}

		while(way->y==(positionZhonx->y+additionY) && way->x==positionZhonx->x+additionX)
		{
			length++;
			positionZhonx->x=way->x;
			positionZhonx->y=way->y;
			oldDote=way;
			way=way->next;
			free(oldDote);
		}
		move_zhonx(orientaionToGo,&positionZhonx->orientation,length);
		new_cell(see_walls(),maze,*positionZhonx);
		if (zhonx_settings.color_sensor_enabled==true)
		{
			if ((zhonx_settings.threshold_greater==false && hal_sensor_get_color(app_context.sensors) < zhonx_settings.threshold_color)
					||
					(zhonx_settings.threshold_greater==true && hal_sensor_get_color(app_context.sensors) > zhonx_settings.threshold_color))
			{
					zhonx_settings.x_finish_maze=positionZhonx->x;
					zhonx_settings.y_finish_maze=positionZhonx->y;
					*endX=positionZhonx->x;
					*endY=positionZhonx->y;
					return;
			}
		}
	}
}

void move_zhonx (int direction_to_go, char *direction_robot, int numberOfCase)
{
	int turn=(4+direction_to_go-*direction_robot)%4;
	*direction_robot=direction_to_go;
	switch (turn)
	{
		case FORWARD :
			break;
		case RIGHT :
				//step_motors_rotate(-90, 90, CHAIN_BEFORE | CHAIN_AFTER);
	//			step_motors_rotate(-90, 90, 0);
				step_motors_rotate_in_place(-90);
				break;
		case UTURN :
				step_motors_rotate_in_place(180);
				break;
		case LEFT :
//				step_motors_rotate(90, 90, 0);
				step_motors_rotate_in_place(90);

			break;
	}
//	step_motors_move(CELL_LENGTH*numberOfCase, 0, CHAIN_BEFORE | CHAIN_AFTER);
	step_motors_move(CELL_LENGTH*numberOfCase, 0, 0);
}

void moveRealZhonxArc(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way)
{
	bool endMidCase;
	coordinate *oldDote;
	int length;
	char additionY=0;
	char additionX=0;
	char orientaionToGo=NORTH;
	while(way!=null)
	{
		length=0;
		if(way->x==(positionZhonx->x+1) && way->y==positionZhonx->y)
		{
			additionX=1;
			additionY=0;
			orientaionToGo=EAST;
		}
		else if(way->x==(positionZhonx->x-1) && way->y==positionZhonx->y)
		{
			additionX=-1;
			additionY=0;
			orientaionToGo=WEST;
		}
		else if(way->y==(positionZhonx->y-1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=-1;
			orientaionToGo=NORTH;
		}
		else if(way->y==(positionZhonx->y+1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=1;
			orientaionToGo=SOUTH;
		}
		else
		{
			hal_os_sleep(200);
			hal_step_motor_disable();
			hal_ui_clear_scr(app_context.ui);
			hal_ui_display_txt(app_context.ui,0,0,"Error way");
			hal_ui_refresh(app_context.ui);
			while(1);
		}

		while(way->y==(positionZhonx->y+additionY) && way->x==positionZhonx->x+additionX)
		{
			length++;
			positionZhonx->x=way->x;
			positionZhonx->y=way->y;
			oldDote=way;
			way=way->next;
			free(oldDote);
		}
		if (way!=null) // (way==null)
			endMidCase=false;
		else
			endMidCase=true;
		move_zhonx_arc(orientaionToGo,positionZhonx,length,endMidCase);
		if(positionZhonx->midOfCase==true)
			new_cell(see_walls(),maze,*positionZhonx);
	}
}

void testMoveRealZhonx(labyrinthe *maze, positionRobot *positionZhonx, coordinate *way, char *endX, char *endY)
{
	bool endMidCase;
	coordinate *oldDote;
	int length;
	char additionY=0;
	char additionX=0;
	char orientaionToGo=NORTH;
	while(way!=null)
	{
		length=0;
		if(way->x==(positionZhonx->x+1) && way->y==positionZhonx->y)
		{
			additionX=1;
			additionY=0;
			orientaionToGo=EAST;
		}
		else if(way->x==(positionZhonx->x-1) && way->y==positionZhonx->y)
		{
			additionX=-1;
			additionY=0;
			orientaionToGo=WEST;
		}
		else if(way->y==(positionZhonx->y-1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=-1;
			orientaionToGo=NORTH;
		}
		else if(way->y==(positionZhonx->y+1) && way->x==positionZhonx->x)
		{

			additionX=0;
			additionY=1;
			orientaionToGo=SOUTH;
		}
		else
		{
			hal_os_sleep(200);
			hal_step_motor_disable();
			hal_ui_clear_scr(app_context.ui);
			hal_ui_display_txt(app_context.ui,0,0,"Error way");
			hal_ui_refresh(app_context.ui);
			while(1);
		}

		while(way->y==(positionZhonx->y+additionY) && way->x==positionZhonx->x+additionX)
		{
			length++;
			positionZhonx->x=way->x;
			positionZhonx->y=way->y;
			oldDote=way;
			way=way->next;
			free(oldDote);
		}
		if(way!=null)
			endMidCase=false;
		else
			endMidCase=true;
		move_zhonx_arc(orientaionToGo,positionZhonx,length,endMidCase);
		new_cell(see_walls(),maze,*positionZhonx);
		if (zhonx_settings.color_sensor_enabled==true)
		{
			if ((zhonx_settings.threshold_greater==false && hal_sensor_get_color(app_context.sensors) < zhonx_settings.threshold_color)
					||
					(zhonx_settings.threshold_greater==true && hal_sensor_get_color(app_context.sensors) > zhonx_settings.threshold_color)) // si on se trouve sur la case d'arrivée
			{
					zhonx_settings.x_finish_maze=positionZhonx->x;
					zhonx_settings.y_finish_maze=positionZhonx->y;
					*endX=positionZhonx->x;
					*endY=positionZhonx->y;
					return;
			}
		}
	}
}

void move_zhonx_arc (int direction_to_go, positionRobot *positionZhonx, int numberOfCase, bool endMidOfCase)
{
	unsigned char chain=CHAIN_AFTER;
	int distanceToMove=CELL_LENGTH*numberOfCase;
	int turn=(4+direction_to_go-positionZhonx->orientation)%4;

	if (positionZhonx->midOfCase==false)
	{
		chain=chain|CHAIN_BEFORE;
	}

	positionZhonx->orientation=direction_to_go;
	switch (turn)
	{
		case FORWARD :
			break;
		case RIGHT :
				if(positionZhonx->midOfCase==true)
					step_motors_rotate_in_place(-90);
				else
				{
					step_motors_rotate(90, CELL_LENGTH/2, chain);
					distanceToMove-=CELL_LENGTH;
				}
	//			step_motors_rotate(-90, 90, 0);
//				step_motors_rotate_in_place(-90);
				break;
		case UTURN :
				step_motors_rotate_in_place(180);
				break;
		case LEFT :
				if(positionZhonx->midOfCase==true)
					step_motors_rotate_in_place(90);
				else
				{
					step_motors_rotate(-90, CELL_LENGTH/2, chain);
					distanceToMove-=CELL_LENGTH;
				}
//				step_motors_rotate_in_place(90);

			break;
	}
	if (positionZhonx->midOfCase==endMidOfCase)
	{
		/*
		 * distanceToMove-=CELL_LENGTH/2;
		 * distanceToMove+=CELL_LENGTH/2;
		 */
	}
	else if(positionZhonx->midOfCase==true) // so endMidOfCase=false
	{
		distanceToMove-=CELL_LENGTH/2;
	}
	else
	{
		distanceToMove+=CELL_LENGTH/2;
	}
	chain=0;

	if (positionZhonx->midOfCase==false)
	{
		chain=chain | CHAIN_BEFORE;
	}
	if (endMidOfCase==false)
	{

		chain=chain | CHAIN_AFTER;
	}

	step_motors_move(distanceToMove, 0, chain);
	positionZhonx->midOfCase=endMidOfCase;
}

void new_cell(inputs new_walls, labyrinthe *maze,positionRobot positionZhonx)
	{
#ifndef test
	switch(positionZhonx.orientation)
			{
			case NORTH:
				maze->cell[positionZhonx.x][positionZhonx.y].wall_north=new_walls.front;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_east=new_walls.right;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_west=new_walls.left;
				if(positionZhonx.y>0)
					maze->cell[positionZhonx.x][positionZhonx.y-1].wall_south=new_walls.front;
				if(positionZhonx.x<(MAZE_SIZE-1))
					maze->cell[positionZhonx.x+1][positionZhonx.y].wall_west=new_walls.right;
				if(positionZhonx.x>0)
					maze->cell[positionZhonx.x-1][positionZhonx.y].wall_east=new_walls.left;
				break;
			case EAST:
				maze->cell[positionZhonx.x][positionZhonx.y].wall_east=new_walls.front;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_south=new_walls.right;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_north=new_walls.left;
				if(positionZhonx.x<(MAZE_SIZE-1))
					maze->cell[positionZhonx.x+1][positionZhonx.y].wall_west=new_walls.front;
				if(positionZhonx.y<(MAZE_SIZE-1))
					maze->cell[positionZhonx.x][positionZhonx.y+1].wall_north=new_walls.right;
				if(positionZhonx.y>0)
					maze->cell[positionZhonx.x][positionZhonx.y-1].wall_south=new_walls.left;
				break;
			case SOUTH:
				maze->cell[positionZhonx.x][positionZhonx.y].wall_south=new_walls.front;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_west=new_walls.right;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_east=new_walls.left;
				if(positionZhonx.y<(MAZE_MAX_SIZE-1))
					maze->cell[positionZhonx.x][positionZhonx.y+1].wall_north=new_walls.front;
				if(positionZhonx.x>0)
					maze->cell[positionZhonx.x-1][positionZhonx.y].wall_east=new_walls.right;
				if(positionZhonx.x<(MAZE_SIZE-1))
					maze->cell[positionZhonx.x+1][positionZhonx.y].wall_west=new_walls.left;
				break;
			case WEST:
				maze->cell[positionZhonx.x][positionZhonx.y].wall_west=new_walls.front;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_north=new_walls.right;
				maze->cell[positionZhonx.x][positionZhonx.y].wall_south=new_walls.left;
				if(positionZhonx.x>0)
					maze->cell[positionZhonx.x-1][positionZhonx.y].wall_east=new_walls.front;
				if(positionZhonx.y>0)
					maze->cell[positionZhonx.x][positionZhonx.y-1].wall_south=new_walls.right;
				if(positionZhonx.y<(MAZE_SIZE-1))
					maze->cell[positionZhonx.x][positionZhonx.y+1].wall_north=new_walls.left;
				break;
			}
#else
	//print_maze(*maze,x,y);
	if(maze->cell[positionZhonx.x][positionZhonx.y].wall_north==NO_KNOW)
			{
			maze->cell[positionZhonx.x][positionZhonx.y].wall_north=WALL_KNOW;
			if(positionZhonx.y>0)
					{
					maze->cell[positionZhonx.x][positionZhonx.y-1].wall_south=WALL_KNOW;
					}
			}
	if(maze->cell[positionZhonx.x][positionZhonx.y].wall_east==NO_KNOW)
			{
			maze->cell[positionZhonx.x][positionZhonx.y].wall_east=WALL_KNOW;
			if(positionZhonx.x+1<MAZE_SIZE)
					{
					maze->cell[positionZhonx.x+1][positionZhonx.y].wall_west=WALL_KNOW;
					}
			}
	if(maze->cell[positionZhonx.x][positionZhonx.y].wall_south==NO_KNOW)
			{
			maze->cell[positionZhonx.x][positionZhonx.y].wall_south=WALL_KNOW;
			if(positionZhonx.y+1<MAZE_SIZE)
					{
					maze->cell[positionZhonx.x][positionZhonx.y+1].wall_north=WALL_KNOW;
					}
			}
	if(maze->cell[positionZhonx.x][positionZhonx.y].wall_west==NO_KNOW)
			{
			maze->cell[positionZhonx.x][positionZhonx.y].wall_west=WALL_KNOW;
			if(positionZhonx.x>0)
					{
					maze->cell[positionZhonx.x-1][positionZhonx.y].wall_east=WALL_KNOW;
					}
			}

#endif
	}

void poids(labyrinthe *maze, int xFinish, int yfinish, bool wallNoKnow)
	{
	int length=0;
	int x=xFinish;
	int y=yfinish;
	maze->cell[x][y].length=length;
	coordinate *dotes_to_verifie=NULL;
	new_dot(&dotes_to_verifie,x,y);
	coordinate *new_dotes_to_verifie=NULL;
	coordinate *pt=NULL;


	while (dotes_to_verifie!=NULL)
			{
			length++;
			while (dotes_to_verifie!=NULL)
					{
					//printf("x: %2d y:%2d\n",x,y);
					//printf(" %d\n%d %d\n %d\n\n",maze->cell[x][y].wall_north,maze->cell[x][y].wall_west ,maze->cell[x][y].wall_east,maze->cell[x][y].wall_south);
					x=dotes_to_verifie->x;
					y=dotes_to_verifie->y;
					pt=dotes_to_verifie->previous;
					free(dotes_to_verifie);
					dotes_to_verifie=pt;
					if((maze->cell[x][y].wall_north==NO_WALL || (wallNoKnow == true && maze->cell[x][y].wall_north == NO_KNOW)) && maze->cell[x][y-1].length>length-1 && y>0)
							{
							new_dot(&new_dotes_to_verifie,x,y-1);
							maze->cell[x][y-1].length=length;
							}
					if((maze->cell[x][y].wall_east==NO_WALL || (wallNoKnow == true && maze->cell[x][y].wall_east == NO_KNOW)) && maze->cell[x+1][y].length>length && x+1<MAZE_SIZE)
							{
							new_dot(&new_dotes_to_verifie,x+1,y);
							maze->cell[x+1][y].length=length;
							}
					if((maze->cell[x][y].wall_south==NO_WALL || (wallNoKnow == true && maze->cell[x][y].wall_south == NO_KNOW)) && maze->cell[x][y+1].length>length && y+1<MAZE_SIZE)
							{
							new_dot(&new_dotes_to_verifie,x,y+1);
							maze->cell[x][y+1].length=length;
							}
					if((maze->cell[x][y].wall_west==NO_WALL || (wallNoKnow == true && maze->cell[x][y].wall_west == NO_KNOW)) && maze->cell[x-1][y].length>length && x>0)
							{
							new_dot(&new_dotes_to_verifie,x-1,y);
							maze->cell[x-1][y].length=length;
							}
					}
			//print_length(*maze);
			dotes_to_verifie=new_dotes_to_verifie;
			new_dotes_to_verifie=NULL;
			}
	}

void new_dot(coordinate **old_dot,int x,int y)
	{
	//printf("x : %d ",x);
	//printf("y : %d ",y);
	if(*old_dot!=NULL)
			{
			(*old_dot)->next=calloc_s(1,sizeof(coordinate));
			coordinate *pt=*old_dot;
			*old_dot=(*old_dot)->next;
			(*old_dot)->previous=pt;
			}
	else
			{
			(*old_dot)=(coordinate*)calloc_s(1,sizeof(coordinate));
			}
	(*old_dot)->x=x;
	(*old_dot)->y=y;
	}

void maze_init (labyrinthe *maze)
	{
#ifndef test
	for(int i=0; i<MAZE_SIZE; i++)
			{
			for(int y=0; y<MAZE_SIZE; y++)
					{
					maze->cell[i][y].wall_north=NO_KNOW;
					maze->cell[i][y].wall_west=NO_KNOW;
					maze->cell[i][y].wall_south=NO_KNOW;
					maze->cell[i][y].wall_east=NO_KNOW;
					maze->cell[i][y].length=2000;
					}
			}
	for(int i=0; i<16; i++)
			{
			maze->cell[i][0].wall_north=WALL_KNOW;
			maze->cell[i][MAZE_SIZE-1].wall_south=WALL_KNOW;
			maze->cell[0][i].wall_west=WALL_KNOW;
			maze->cell[MAZE_SIZE-1][i].wall_east=WALL_KNOW;
			}
#else
	labyrinthe maze_initial= {{{{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_KNOW,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_WALL,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_KNOW,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_KNOW,NO_WALL,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_KNOW,2000},\
								{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_KNOW,NO_WALL,NO_WALL,2000}},\
							{{NO_KNOW,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_WALL,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_WALL,NO_KNOW,NO_KNOW,2000},\
								{NO_WALL,NO_KNOW,NO_KNOW,NO_WALL,2000}}}};

	*maze=maze_initial;
#endif
	}

void print_maze(const labyrinthe maze, const int x_robot, const int y_robot)
	{
	hal_ui_clear_scr(app_context.ui);
	int size_cell_on_oled=((63)/MAZE_SIZE);
	int x,y;
	for (y=0; y<MAZE_SIZE; y++)
			{
			for (x=0; x<MAZE_SIZE; x++)
					{
					if (maze.cell[x][y].wall_north == WALL_KNOW)
							{
							hal_ui_fill_rect(app_context.ui,x*size_cell_on_oled, y*size_cell_on_oled, size_cell_on_oled+1, 1);
							}
					if (maze.cell[x][y].wall_west == WALL_KNOW)
							{
							hal_ui_fill_rect(app_context.ui,x*size_cell_on_oled, y*size_cell_on_oled, 1, size_cell_on_oled+1);
							}
					if (maze.cell[x][y].wall_south == WALL_KNOW)
							{
							hal_ui_fill_rect(app_context.ui,x*size_cell_on_oled, (y+1)*size_cell_on_oled, size_cell_on_oled+1, 1);
							}
					if (maze.cell[x][y].wall_east == WALL_KNOW)
							{
							hal_ui_fill_rect(app_context.ui,(x+1)*size_cell_on_oled, y*size_cell_on_oled, 1, size_cell_on_oled+1);
							}
					}
			}
	//print_length(maze);
	hal_ui_fill_rect(app_context.ui,x_robot*size_cell_on_oled, y_robot*size_cell_on_oled, size_cell_on_oled, size_cell_on_oled);
	hal_ui_refresh(app_context.ui);
	}

void* calloc_s (size_t nombre, size_t taille)
	{
	void* pt=calloc(nombre,taille);
	if (pt==NULL)
			{
			printf("null pointer exception, full memory");
			while (1);
			}
	return pt;
	}

void print_length(const labyrinthe maze)
	{
	printf("  ");
	for (int i=0; i<MAZE_SIZE; i++)
			{
			printf("%4d",i);
			}
	printf("\n\n");
	for(int i=0; i<MAZE_SIZE; i++)
			{
			printf("%2d ", i);
			for(int j=0; j<MAZE_SIZE; j++)
					{
					if(maze.cell[j][i].wall_north==WALL_KNOW)
							{
							printf("====*");
							}
					else
							{
							printf("    *");
							}
					}
			printf("\n   ");
			for(int j=0; j<MAZE_SIZE; j++)
					{
					printf("%4d",maze.cell[j][i].length);
					if(maze.cell[j][i].wall_east==WALL_KNOW)
							{
							printf("|");
							}
					else
							{
							printf(" ");
							}
					}
			printf("\n");
			}
	printf("\n");
	}

void clearMazelength(labyrinthe* maze)
	{
	int x,y;
	for(y=0; y<MAZE_SIZE; y++)
			{
			for(x=0; x<MAZE_SIZE; x++)
					{
					maze->cell[x][y].length=2000;
					}
			}
	}

inputs see_walls ()
{
	unsigned char   sensors_state = hal_sensor_get_state(app_context.sensors);
	inputs walls={NO_WALL,NO_WALL,NO_WALL};
	if (check_bit(sensors_state, SENSOR_L10_POS) == false)
	   {
		   walls.left=WALL_KNOW;
	   }
	if (check_bit(sensors_state, SENSOR_R10_POS) == false)
	   {
		   walls.right=WALL_KNOW;
	   }
	if (check_bit(sensors_state, SENSOR_F10_POS) == false)
	   {
		   walls.front=WALL_KNOW;
	   }

	return walls;
}

bool mini_way_find(labyrinthe *maze,char xStart, char yStart, char xFinish, char yFinish)
{
	// TODO ne pas comparer les distance mais les chemins --> normalement fait : a vérifier
	// TODO trouver non pas le chemain le plus court mais le chemain le plus rapide
	coordinate *way1=null;
	coordinate *way2=null;
	clearMazelength(maze);
	poids(maze,xFinish,yFinish,true);
	moveVirtualZhonx(*maze,(positionRobot){true,xStart,yStart,NORTH},way1,xFinish,yFinish);
	clearMazelength(maze);
	poids(maze,xFinish,yFinish,false);
	moveVirtualZhonx(*maze,(positionRobot){true,xStart,yStart,NORTH},way2,xFinish,yFinish);
	hal_ui_clear_scr(app_context.ui);
	bool waySame=diffWay(way1,way2);
	switch (waySame) {
		case true:
			hal_ui_display_txt(app_context.ui,0,20,"2 way = : yes");
			break;
		case false:
			hal_ui_display_txt(app_context.ui,0,20,"2 way = : no");
			break;
	}
	deletWay(way1);
	deletWay(way2);
	hal_ui_refresh(app_context.ui);
	hal_os_sleep(3000);
	return (waySame);
}
bool diffWay(coordinate *way1,coordinate *way2)
{
	while(way1!=null&&way2!=null)
	{
		if(way1->x!=way2->x||way1->y!=way2->y)
		{
			return false;
		}
		way1=way1->next;
		way2=way2->next;
	}
	if(way1!=null||way2!=null)
	{
		return false;
	}
	return true;
}
void deletWay(coordinate *way) // TODO: verifier la fonction
{
	while (way!=null)
	{
		way=way->next;
		free(way->previous);
	}
}
void clearPartLenght(labyrinthe *maze,char x, char y)//TODO : terminer cette fonction
{
	coordinate *newDotesToVerifie=null;
	coordinate *DotesToVerifie=null;
	coordinate *pt=null;
	int length=0;
	/*
	 * première étape, on vérifie si il n'y a pas des distance unferieur autour du robot
	 * dans le cas ou il y a des distance inferieur on note le point a comme un poin à
	 * partir duquel on verifie les distance inferiieur
	 *
	 */
	if(maze->cell[x][y].wall_east==WALL_KNOW)
	{
		if(maze->cell[x][y].length == maze->cell[x+1][y].length+1)
		{
			maze->cell[x+1][y].length=2000;
			new_dot(&DotesToVerifie,x+1,y);
		}
	}
	if(maze->cell[x][y].wall_west==WALL_KNOW)
	{
		if(maze->cell[x][y].length == maze->cell[x-1][y].length+1)
		{
			maze->cell[x+1][y].length=2000;
			new_dot(&DotesToVerifie,x-1,y);
		}
	}
	if(maze->cell[x][y].wall_north==WALL_KNOW)
	{
		if(maze->cell[x][y].length == maze->cell[x][y-1].length+1)
		{
			maze->cell[x+1][y].length=2000;
			new_dot(&DotesToVerifie,x,y-1);
		}
	}
	if(maze->cell[x][y].wall_south==WALL_KNOW)
	{
		if(maze->cell[x][y].length == maze->cell[x][y+1].length+1)
		{
			maze->cell[x+1][y].length=2000;
			new_dot(&DotesToVerifie,x,y-1);
		}
	}
	while (DotesToVerifie!=NULL)
	{
	length++;
	while (DotesToVerifie!=NULL)
			{
			//printf("x: %2d y:%2d\n",x,y);
			//printf(" %d\n%d %d\n %d\n\n",maze->cell[x][y].wall_north,maze->cell[x][y].wall_west ,maze->cell[x][y].wall_east,maze->cell[x][y].wall_south);
			x=DotesToVerifie->x;
			y=DotesToVerifie->y;
			pt=(void*)DotesToVerifie->previous;
			free(DotesToVerifie);
			DotesToVerifie=pt;
			if((maze->cell[x][y].wall_north==NO_WALL || maze->cell[x][y].wall_north == NO_KNOW) && maze->cell[x][y-1].length==length+1 && y>0)
					{
					new_dot(&newDotesToVerifie,x,y-1);
					maze->cell[x][y-1].length=2000;
					}
			if((maze->cell[x][y].wall_east==NO_WALL || maze->cell[x][y].wall_east == NO_KNOW) && maze->cell[x+1][y].length==length+1 && x+1<MAZE_SIZE)
					{
					new_dot(&newDotesToVerifie,x+1,y);
					maze->cell[x+1][y].length=2000;
					}
			if((maze->cell[x][y].wall_south==NO_WALL || maze->cell[x][y].wall_south == NO_KNOW) && maze->cell[x][y+1].length==length+1 && y+1<MAZE_SIZE)
					{
					new_dot(&newDotesToVerifie,x,y+1);
					maze->cell[x][y+1].length=2000;
					}
			if((maze->cell[x][y].wall_west==NO_WALL || maze->cell[x][y].wall_west == NO_KNOW) && maze->cell[x-1][y].length==length+1 && x>0)
					{
					new_dot(&newDotesToVerifie,x-1,y);
					maze->cell[x-1][y].length=2000;
					}
			length++;
			}
	print_length(*maze);
	DotesToVerifie=newDotesToVerifie;
	newDotesToVerifie=NULL;
	}
}

void waitStart()
{
	unsigned char sensors_state = hal_sensor_get_state(app_context.sensors);
	while(check_bit(sensors_state, SENSOR_F10_POS)==true)
		sensors_state = hal_sensor_get_state(app_context.sensors);
	hal_os_sleep(200);
	while(check_bit(sensors_state, SENSOR_F10_POS)==false)
		sensors_state = hal_sensor_get_state(app_context.sensors);
}

void calibrateSimple()
{
	hal_step_motor_enable();
	char orientation=0;
	unsigned char sensors_state =0;
	for(int i=0; i<2;i++)
	{
		sensors_state =hal_sensor_get_state(app_context.sensors);
		if (check_bit(sensors_state, SENSOR_L10_POS) == false)
		{
			goOrientation(&orientation,orientation-1);
		}
		else if (check_bit(sensors_state, SENSOR_R10_POS) == false)
		{
			goOrientation(&orientation,orientation+1);
		}
 		step_motors_basic_move(70);
		hal_os_sleep(500);
		step_motors_basic_move(-((CELL_LENGTH/2)-46));
	}
	goOrientation(&orientation,0);
	hal_os_sleep(100);
	hal_step_motor_disable();

}

void goOrientation(char *orientationZhonx, char directionToGo)
{
	int turn=(4+directionToGo-*orientationZhonx)%4;
	*orientationZhonx=directionToGo;
	switch (turn)
	{
		case FORWARD :
			break;
		case RIGHT :
				step_motors_rotate_in_place(-90);
				break;
		case UTURN :
				step_motors_rotate_in_place(180);
				break;
		case LEFT :
				step_motors_rotate_in_place(90);

			break;
	}
}

void doUTurn(positionRobot *positionZhonx)
{
	hal_step_motor_enable();
	goOrientation(&positionZhonx->orientation, (positionZhonx->orientation+2)%4);
	hal_step_motor_disable();
}
