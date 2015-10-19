/*
 * bezier_curves.c
 *
 *  Created on: 24 mars 2015
 *      Author: Colin
 */

#include <math.h>
#include "app/bezier_curves.h"
#include "oled/ssd1306.h"
extern void* calloc_s (size_t nombre, size_t taille);

void bezierCurve3 (point *pointA,point *pointB,point *pointC,point *pointD,point *tableOfPoint,int numberOfStep)
{
    unsigned int i;
    for (i=0; i <= numberOfStep; ++i)
    {
        double t = (double)i / (double)numberOfStep;

        double a = pow((1.0 - t), 3.0);
        double b = 3.0 * t * pow((1.0 - t), 2.0);
        double c = 3.0 * pow(t, 2.0) * (1.0 - t);
        double d = pow(t, 3.0);

        double x = a * pointA->x + b * pointB->x + c * pointC->x + d * pointD->x;
        double y = a * pointA->y + b * pointB->y + c * pointC->y + d * pointD->y;
        tableOfPoint[i].x = x;
        tableOfPoint[i].y = y;
    }
}
void bezierCurve2 (point *pointA,point *pointB,point *pointC,point *tableOfPoint,int numberOfStep)
{
    unsigned int i;
    for (i=0; i <= numberOfStep; ++i)
    {
    	double t = (double)i / (double)numberOfStep;
		double a = pow((1.0 - t), 2.0);
		double b = 2.0 * t * (1.0 - t);
		double c = pow(t, 2.0);
		double x = a * pointA->x + b * pointB->x + c * pointC->x;
		double y = a * pointA->y + b * pointB->y + c * pointC->y;
		tableOfPoint[i].x = x;
		tableOfPoint[i].y = y;
    }
}
point* bezierCurves90(int *length)
{
	static point *listOfPoint=NULL;
	*length=NUMBER_OF_STEP_BY_90;
	if(listOfPoint==NULL)
	{
		listOfPoint = calloc_s(*length,sizeof(point));
		point pointA = {0,0};
		point pointB = {0,64};
		point pointC = {64,64};
		bezierCurve2 (&pointA,&pointB,&pointC,listOfPoint,*length);
	}
	return listOfPoint;
}
void test1Bezier()
{
	int i=0;
	point pointA = {0,0};
	point pointB = {0,64};
	point pointC = {64,64};
	while (1)
	{

		point listOfPoint[101];
		bezierCurve2 (&pointA,&pointB,&pointC,listOfPoint,100);
		printCurve(listOfPoint,100);
		i=1;
		while(i);
	}
}
void test2Bezier()
{
	int i;
	point pointA = {0,0};
	point pointB = {0,64};
	point pointC = {64,64};
	point pointD = {64,0};
	while (1)
	{

		point listOfPoint[(int)(NUMBER_OF_STEP_BY_180)];
		bezierCurve3 (&pointA,&pointB,&pointC,&pointD,listOfPoint,NUMBER_OF_STEP_BY_180);
		printCurve(listOfPoint,NUMBER_OF_STEP_BY_180);
	}
}
void printCurve (point *listOfPoint,int lenght)
{
	int i;
	ssd1306ClearScreen();
	for ( i = 0; i <= lenght; i++)
	{
		ssd1306DrawPixel(listOfPoint[i].x,listOfPoint[i].y);
	}
	ssd1306Refresh();
}
