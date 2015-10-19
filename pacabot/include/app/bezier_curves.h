/*
 * bezier_curves.h
 *
 *  Created on: 25 mars 2015
 *      Author: zhonx
 */

#ifndef BEZIER_CURVES_H_
#define BEZIER_CURVES_H_

#define THICKNESS_OF_THE_WALL			12 //in millimeter
#define LENGHT_OF_THE_WALL 				168

#define RAYON_ARC						(THICKNESS_OF_THE_WALL+LENGHT_OF_THE_WALL)/2
#define NUMBER_OF_STEP_BY_DEGRES		(M_PI*RAYON_ARC)/(360*2)

#define NUMBER_OF_STEP_BY_45		45*NUMBER_OF_STEP_BY_DEGRES
#define NUMBER_OF_STEP_BY_90		90*NUMBER_OF_STEP_BY_DEGRES
#define NUMBER_OF_STEP_BY_135	135*NUMBER_OF_STEP_BY_DEGRES
#define NUMBER_OF_STEP_BY_180	180*NUMBER_OF_STEP_BY_DEGRES

typedef struct
{
	char x;
	char y;
}point;

void bezierCurve3 (point *pointA,point *pointB,point *pointC,point *pointD,point *tableOfPoint,int numberOfStep);
void bezierCurve2 (point *pointA,point *pointB,point *pointC,point *tableOfPoint,int numberOfStep);
point* bezierCurves90(int *length);
void printCurve (point *listOfPoint,int lenght);

#endif /* BEZIER_CURVES_H_ */
