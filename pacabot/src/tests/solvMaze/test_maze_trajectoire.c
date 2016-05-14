// Programme de test permettant de simuler une trajectoire � partir d'une
// chaine de commande
/*
 * En attente des capteurs droit et gauche pour le recallage en ligne droite
 * La chaine de caractere permet de simuler la direction que l'on prend
 * pour sortir du labyrinthe
 *
 * Modification : Bertrand 29/04/2012
 */
#include <math.h>
#include "config/basetypes.h"
#include "config/config.h"

#include "hal/hal_os.h"
#include "hal/hal_serial.h"
#include "hal/hal_step_motor.h"

#include "drivers/step_motor_driver.h"

#include <string.h>

HAL_STEP_MOTOR_HANDLE step_motor1;
HAL_STEP_MOTOR_HANDLE step_motor2;
HAL_SERIAL_HANDLE serial;

void ChaineParcours(char * Tab, int taille);
void Tourne90(int sens);
void AvanceCase(int dist);

extern void trajectoire1(char * Tab, int taille);
extern void trajectoire2(char * Tab, int taille);

int test_maze_trajectoire(void)
{
    unsigned char   port = 0;
    char chemin[]="0012323021010322";  // le premier 0 correspond � la position de depart
    hal_serial_open(&serial, &port, 115200);
    step_motors_init();
    HAL_Delay(2000);

	//ChaineParcours(chemin,strlen(chemin));
    trajectoire1(chemin, strlen(chemin));

    hal_step_motor_disable();
	return 0;
}

void ChaineParcours(char * Tab, int taille)
{
	int ii=1;
	Tourne90(Tab[ii]-Tab[ii-1]);
	ii++;
	int dist=0;
	while (ii<taille)
	{
		Tourne90(Tab[ii]-Tab[ii-1]);
		dist=1; ii++;
		while (Tab[ii]==Tab[ii-1])
		{
			dist++; ii++;
		}
		AvanceCase(dist);
	}
}

void Tourne90(int sens)
{
    int dist;
    if (sens !=0)
    {
		if (sens>0)
		{
			hal_step_motor_set_direction(step_motor1, 0);
			hal_step_motor_set_direction(step_motor2, 1);
		}
		else if (sens<0)
		{
			hal_step_motor_set_direction(step_motor1, 1);
			hal_step_motor_set_direction(step_motor2, 0);
		}

		dist=hal_step_motor_get_counter(step_motor1)+2785;
		while (hal_step_motor_get_counter(step_motor1) < dist);

		hal_step_motor_set_direction(step_motor1, 1);
		hal_step_motor_set_direction(step_motor2, 1);
	}
	//
}
void AvanceCase(int nbcase)
{
    int dist;
    unsigned char   readChar=0;
    unsigned int    nbytes = 1;
    int Freq=1000;

    int freine=0;
    int freineD=0;
    int freineG=0;

    hal_step_motor_set_freq(step_motor1, Freq);
    hal_step_motor_set_freq(step_motor2, Freq);
    dist = hal_step_motor_get_counter(step_motor2)+2785*nbcase;
    while (hal_step_motor_get_counter(step_motor2) < dist)
    {
        freine = Freq / 3;
    	hal_serial_read(serial, &readChar, &nbytes, 1);
    	if (readChar == 'e')
    	{
    		freineD=freine;
    		freineG=0;
    	}

    	if (readChar == 'z')
    	{
    		freineD=0;
    		freineG=freine;
    		hal_step_motor_set_freq(step_motor1, Freq);
    		hal_step_motor_set_freq(step_motor2, Freq-freine);
    	}
		hal_step_motor_set_freq(step_motor1, Freq-freineD);
		hal_step_motor_set_freq(step_motor2, Freq-freineG);

    	if (Freq < 5000) Freq++;
    }
}
void essai3(char * Tab, int taille)
{
	int ii=1;
	//int dist=0;
	//int tourne=0;
	int tab_motor1[20];
	int tab_motor2[20];
    /* Compute the total distance in steps */
	long distance_steps = 180 * STEPS_PER_MM * 2;
    long distance_rot = 30 * STEPS_PER_MM * 2;

	tab_motor1[0]=hal_step_motor_get_counter(step_motor1);
	tab_motor2[0]=hal_step_motor_get_counter(step_motor2);
	for (ii=1;ii<taille;ii++)
	{
		if (Tab[ii]-Tab[ii-1]==0)
		{
			tab_motor1[ii]=tab_motor1[ii-1]+ distance_steps;
			tab_motor2[ii]=tab_motor2[ii-1]+ distance_steps;
		}
		if  (Tab[ii]-Tab[ii-1]>0)
		{
			tab_motor1[ii]=tab_motor1[ii-1] + distance_steps;
			tab_motor1[ii-1] = tab_motor1[ii-1] - distance_rot;
			tab_motor2[ii]=tab_motor2[ii-1];
		}
		if  (Tab[ii]-Tab[ii-1]<0)
		{
			tab_motor2[ii]=tab_motor2[ii-1] + distance_steps;
			tab_motor2[ii-1] = tab_motor2[ii-1] - distance_rot;
			tab_motor2[ii]=tab_motor2[ii-1];
		}
	}

	hal_step_motor_set_direction(step_motor1, 1);
	hal_step_motor_set_direction(step_motor2, 1);
    int Freq=2000;

    long dist1 = 0;
    long dist2 = 0;

    for (ii=0; ii<taille; ii++)
    {
        hal_step_motor_set_freq(step_motor1, Freq);
        hal_step_motor_set_freq(step_motor2, Freq);

		while (dist1 < tab_motor1[ii] &&
			   dist2 < tab_motor2[ii])
		{
			dist1 = hal_step_motor_get_counter(step_motor1);
			dist2 = hal_step_motor_get_counter(step_motor2);
			if (dist1 > tab_motor1[ii])
				hal_step_motor_set_freq(step_motor1, 10);
			if (dist2 > tab_motor2[ii])
				hal_step_motor_set_freq(step_motor2, 10);
		}
    }
}


