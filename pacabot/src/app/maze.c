/*---------------------------------------------------------------------------
 *
 *      maze.c
 *
 *---------------------------------------------------------------------------*/

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

#include "stm32f4xx.h"

#define NORD  0
#define EST   1
#define SUD   2
#define OUEST 3

#define DROITE 0
#define GAUCHE 1
#define DEVANT 2

#define PIX_AFF 8
#define PIX_AFF_HOST 24

const char  _porte[]    = {1,2,4,8};
const int   _dx[]       = {0,  1, 0,-1};
const int   _dy[]       = {-1, 0, 1, 0};
int         _trouve     = 0;
int         _increment  = 0;
int         _recule     = 0;

// String containing the commands of the explored maze
char _ChaineCommande[(MAZE_MAX_SIZE * MAZE_MAX_SIZE) / 4];
int _CmdSize;

extern maze_object stored_mazes[5];
extern maze_object *current_maze;


//#define USE_SOFT_BREAKS
#ifndef USE_SOFT_BREAKS
# define WAIT_VALIDATION
#else
# define WAIT_VALIDATION \
{ \
    unsigned int    nbytes = 1; \
    unsigned char   readChar = 0; \
    printf("Validation Required\n"); \
    do \
    { \
        hal_serial_read(app_context.serial, &readChar, &nbytes, FOREVER); \
    } \
    while (readChar != 13); \
}
#endif


/* Static functions */
static void display(char tab[][MAZE_MAX_SIZE], int indice, int sens);
static void getPosition(unsigned char X, unsigned char Y,
                        unsigned char sens, char table[][MAZE_MAX_SIZE]);
static void setPosition(unsigned char X, unsigned char Y, unsigned char sens,
                        char table[][MAZE_MAX_SIZE], unsigned char *Tab_capt);
static void commande(int dir, int dist);
static void trajectoire1(char *Tab, int taille);
static void trajectoire2(char *Tab, int taille);
static void trajectoire3(char *Tab, int taille);
static void Marche_arriere(int direction);
static unsigned char get_abs_position(int angle);
static int parcours(char table[][MAZE_MAX_SIZE], unsigned char X,
                    unsigned char Y, unsigned char sens);
static int robot_calibration(char table[][MAZE_MAX_SIZE],
                             unsigned char x, unsigned char y,
                             unsigned int *sens, bool setPos);
static int calibrate(int direction, bool setPos, char table[][MAZE_MAX_SIZE],
                     unsigned char x, unsigned char y, unsigned int *sens);
static void maze_init(void);

static int wait_validation(unsigned long timeout)
{
    timeout += hal_os_get_systicks();
    do
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET)
        {
            // Wait until button is released
            while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET);
            hal_os_sleep(200);
            return 0;
        }

        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) != Bit_SET)
        {
            // Wait until button is released
            while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) != Bit_SET);
            hal_os_sleep(200);
            return -2;
        }
    } while (timeout > hal_os_get_systicks());
    return -1;
}


// Color sensor calibration
int sensor_calibrate(void)
{
    int             rv;
    int 			i=0;
    unsigned long	arrival_color = 30000;
    unsigned long	area_color = 500000;
    char            str[100];

    while (1)
    {
        hal_ui_clear_scr(app_context.ui);
        hal_ui_display_txt(app_context.ui, 0, 9, "Present arrival color");
        hal_ui_display_txt(app_context.ui, 0, 64 - 9, "'RIGHT' TO VALIDATE");
        hal_ui_refresh(app_context.ui);

        arrival_color = hal_sensor_get_color(app_context.sensors);
        sprintf(str, "Color sens: %i", arrival_color);
        hal_ui_display_txt(app_context.ui, 10, 18, str);

        hal_ui_refresh(app_context.ui);

        rv = wait_validation(500);
        if (rv == 0)
        {
            // Value validated
        	for (i=0; i<100;i++)
        	{
        		arrival_color+=hal_sensor_get_color(app_context.sensors);
        		hal_os_sleep(50);
        	}
        	arrival_color/=i;
            sprintf(str, "Value %i validated", arrival_color);
            hal_ui_clear_scr(app_context.ui);
            hal_ui_display_txt(app_context.ui, 2, 9, str);
            hal_ui_refresh(app_context.ui);
            hal_os_sleep(1000);
            break;
        }
        else if (rv == -2)
        {
            // User aborted
            hal_ui_clear_scr(app_context.ui);
            hal_ui_display_txt(app_context.ui, 2, 9, "Calibration aborted");
            hal_ui_refresh(app_context.ui);
            hal_os_sleep(1000);
            return 0;
        }
    }

    while (1)
    {
        hal_ui_clear_scr(app_context.ui);
        hal_ui_display_txt(app_context.ui, 0, 9, "Present area color");
        hal_ui_display_txt(app_context.ui, 0, 64 - 9,
                           "'RIGHT' TO VALIDATE");
        hal_ui_refresh(app_context.ui);

        area_color = hal_sensor_get_color(app_context.sensors);
        sprintf(str, "Color sens: %i", area_color);
        hal_ui_display_txt(app_context.ui, 10, 18, str);

        hal_ui_refresh(app_context.ui);

        rv = wait_validation(500);
        if (rv == 0)
        {
            // Value validated
        	for (i=0; i<100;i++)
        	{
        		area_color+=hal_sensor_get_color(app_context.sensors);
        		hal_os_sleep(50);
        	}
        	area_color/=i;
            sprintf(str, "Value %i validated", area_color);
            hal_ui_clear_scr(app_context.ui);
            hal_ui_display_txt(app_context.ui, 2, 9, str);
            hal_ui_refresh(app_context.ui);
            hal_os_sleep(1000);
            break;
        }
    }

    zhonx_settings.threshold_color = (MAX(arrival_color, area_color) - \
                                      MIN(arrival_color, area_color)) / 2;
    sprintf(str,"diff coul : %d",zhonx_settings.threshold_color);
    hal_ui_clear_scr(app_context.ui);
    hal_ui_display_txt(app_context.ui,1,1,str);
    hal_ui_refresh(app_context.ui);
    hal_os_sleep(2000);
    zhonx_settings.threshold_color += MIN(arrival_color, area_color);
    zhonx_settings.threshold_greater = (arrival_color > area_color);

    return 0;
}

/* Initializes maze context */
static void maze_init(void)
{
    _trouve     = 0;
    _increment  = 0;
    _recule     = 0;
}

// FONCTION PRINCIPALE DE DEPLACEMENT DANS LE LABYRINTHE
extern int mazeBertrand(int i, int str)
{
    int             rv = 0;
    int             cmd_size = 0;
    unsigned int    x = MAZE_MAX_SIZE/2;             // Position de depart par defaut
    unsigned int    y = MAZE_MAX_SIZE/2;
    unsigned int    sens = NORD;       // Direction privilgie de departd
    short           magic_world = 0xBEEF;
	char            labyrinthe [MAZE_MAX_SIZE][MAZE_MAX_SIZE] =
                    {{'0'}};

	// Initialize maze variables
	maze_init();

	ConvertBinaire(labyrinthe);

    // Positionnement
    //----------------
    hal_ui_clear_scr(app_context.ui);
    hal_ui_display_txt(app_context.ui, 13, 1, "POSITIONNER");
    hal_ui_refresh(app_context.ui);
	hal_os_sleep(2000);

    //Reconnaissance du labyrinthe
    //----------------------------
    printf("Debug(Commencement du parcours)\n");
    hal_ui_clear_scr(app_context.ui);
    hal_ui_display_txt(app_context.ui, 13, 1, "START");
    hal_ui_refresh(app_context.ui);

    // Activation des moteurs
    hal_step_motor_enable();

    robot_calibration(labyrinthe, x, y, &sens, true);
    bip4();

    // TODO: Attendre ordre de depart

    // Parcours du labyrinthe
    //-----------------------
    cmd_size = parcours(labyrinthe, x, y, sens);

    _CmdSize =  cmd_size;

    hal_step_motor_disable();

    // Check if a maze is already stored in this area
    if (stored_mazes[0].magic_word == 0xBEEF)
    {
        hal_nvm_init_sector(app_context.nvm, (unsigned long)stored_mazes);
    }
    // Store the maze and the command string into the flash memory
    hal_nvm_write(app_context.nvm, stored_mazes[0].maze,
                  labyrinthe, sizeof(labyrinthe));
    hal_nvm_write(app_context.nvm, stored_mazes[0].cmd_string,
                  _ChaineCommande, sizeof(_ChaineCommande));
    hal_nvm_write(app_context.nvm, &stored_mazes[0].cmd_size,
                  &cmd_size, sizeof(int));
    // Write the magic word
    hal_nvm_write(app_context.nvm, &stored_mazes[0].magic_word, &magic_world, 2);

    current_maze = &stored_mazes[0];

    do
    {
        rv = display_trajectory_menu();
    }
    while (rv > 0);

    return cmd_size + 1;
}


void run_trajectory(int trajectory_nb)
{
    unsigned int    direction = 0;
    unsigned char   sensors_state;
    unsigned long   start;
    unsigned long   run_time;
    char            str[30];

    if ((trajectory_nb <= 0) || (current_maze == null))
    {
        return;
    }

    hal_step_motor_enable();
    //robot_calibration(current_maze->maze, 0, 0, &direction, false);
    commande(_ChaineCommande[1] - _ChaineCommande[0], 0);
    bip4();

    /* Wait while the robot detects something in front of it */
    do
    {
        sensors_state = hal_sensor_get_state(app_context.sensors);
    }
    while (check_bit(sensors_state, SENSOR_F10_POS) == false);


    start = hal_os_get_systicks();

//    switch (trajectory_nb) //PLF FLASH
//    {
//        case 1:
//            trajectoire1(current_maze->cmd_string, current_maze->cmd_size); //PLF
//            break;
//        case 2:
//            trajectoire2(current_maze->cmd_string, current_maze->cmd_size);
//            break;
//        case 3:
//            trajectoire3(current_maze->cmd_string, current_maze->cmd_size);
//            break;
//        default:
//            return;
//    }
    switch (trajectory_nb)
    {
        case 1:
            trajectoire1(_ChaineCommande, _CmdSize); //PLF
            break;
        case 2:
        	trajectoire2(_ChaineCommande, _CmdSize); //PLF
            break;
        case 3:
        	trajectoire3(_ChaineCommande, _CmdSize); //PLF
            break;
        default:
            return;
    }

    run_time = hal_os_get_systicks() - start;
    hal_step_motor_disable();

    sprintf(str, "Run time: %is%i",
            (int)(run_time / 1000), (int)(run_time % 1000));
    hal_ui_display_prompt(app_context.ui, "TRAJECTORY FINISHED", str);
}


// Permet de faire le chemin inverse pour retourner sur la prochaine case  explorer
static void Marche_arriere(int direction)
{

	int taille=1;
	char tab[10];
	int ii;
	if (_increment-_recule<10)
	{
	   for (ii=_recule; ii>_increment; ii--)
	   {
		   tab[taille]=_ChaineCommande[ii] + 2;
		   taille++;
	   }
	   tab[taille] = direction;
	   tab[0]=tab[1];
	   commande(2, 0); // faire demi tour.
	   trajectoire1(tab, taille + 1);
	}
	else
	{
	   commande(6,0);  // redimensionner le tableau car le nombre
	                   // de cases de marche arriere depasse 10
	}
}


static int parcours(char table[][MAZE_MAX_SIZE], unsigned char X,
                    unsigned char Y, unsigned char sens)
{
    int direction;

    if (_recule == 0) // si >0 on est en mode recule (on desactive la prise en compte des capteurs
    {
        table[Y][X] = table[Y][X] | 0x10;
        // Interrogation des capteurs pour dterminer si il y a un mur devant/droite/gauche
        getPosition(X, Y, sens, table); // si on avance (_recule=0) on lit les capteurs
    }
//    display(table, 20, sens);

    // est-on sur la case de fin ?
    //---------------------------------------------------------------------------------------------------
    if (zhonx_settings.color_sensor_enabled)
    {
        if (zhonx_settings.threshold_greater)
        {
            // Threshold color value is bigger than current value
            if (hal_sensor_get_color(app_context.sensors) > zhonx_settings.threshold_color)
            {
                _ChaineCommande[_increment + 1] = sens;
                return (_increment + 1);
            }
        }
        else if (hal_sensor_get_color(app_context.sensors) < zhonx_settings.threshold_color)
        {
            // Threshold color value is smaller than current value
            _ChaineCommande[_increment + 1] = sens;
            return (_increment + 1);
        }
    }
    else if (table[Y][X] == 0x10) // A remplacer dans le cas ou la case d'arrivee est a l'interieur du labyrinthe
    {
        _ChaineCommande[_increment + 1] = sens;  // mettre  ce moment la un test du capteur sol
        return (_increment + 1); // si <> 0 on a trouv la sortie. on renvoit le nombre de cases
                                 // depuis la case de dpart pour sortir du labyrinthe
    }
    //---------------------------------------------------------------------------------------------------

    if (!_trouve) // On a pas encore trouv l'arrive, on parcours selon les 4 directions
    {
        char porte;
        unsigned char tmpX;
        unsigned char tmpY;
        for (direction = 0; direction <= OUEST; direction++)
        {
            if (!_trouve)
            {
                porte = (table[Y][X] & _porte[direction]);  // Test N, E, S, W
                if (porte == 0)
                {                     //0= pas de mur!
                    tmpX = (X + _dx[direction]);     // Calcul de la prochaine position
                    tmpY = (Y + _dy[direction]);
                    porte = (table[tmpY][tmpX]) & 0x10; // Pour tester si on est dj pass par l
                    if (porte == 0)
                    {
                        // c'est bon on est pas encore pass par l
                        int tourner = direction - _ChaineCommande[_increment]; // calcul de la rotation  envoyer;
                        int avancer = 1;

                        if (_recule > 0)      // on a eu une squence de marche arriere qu'il va falloir convertir
                        {                   // en marche avant
                            Marche_arriere(direction);
                            _recule = 0;
                            tourner = 0;
                            avancer = 0;
                        }
                        _increment++;               // La longueur du chemin pour aller vers la sortie s'incrmente
                        _ChaineCommande[_increment] = direction;       // Enregistrer la nouvelle orientation
                        commande(tourner, avancer);
                        _trouve = parcours(table, tmpX, tmpY, direction);
                    }
                }
            }
        }
        if (!_trouve)
        {
            // Si on ne veut pas que le robot retourne au depart tout en marche arriere depuis l'arrivee
            if (_recule == 0)
            {
                _recule = _increment;
            }
            //	int tourner=_ChaineCommande[_increment-1]-_ChaineCommande[_increment];;
            //	int avancer=-1;
            _increment--;
            //	table[Y][X] = table[Y][X]-0x10; // on efface la trace de notre passage
            //	commande( tourner,avancer);
        }
    }
    return _trouve;
}


// Permet de dterminer ou sont les murs
static void getPosition(unsigned char X, unsigned char Y,
                        unsigned char sens, char table[][MAZE_MAX_SIZE])
{
   unsigned char mur[3];
   unsigned char sensors_state;
   sensors_state = hal_sensor_get_state(app_context.sensors);

   mur[DROITE] = 0;
   mur[GAUCHE] = 0;
   mur[DEVANT] = 0;
   // Recuperation des informations des capteurs
   if (check_bit(sensors_state, SENSOR_L10_POS) == false)
   {
	   mur[GAUCHE] = 1;
   }
   if (check_bit(sensors_state, SENSOR_R10_POS) == false)
   {
	   mur[DROITE] = 1;
   }
   if (check_bit(sensors_state, SENSOR_F10_POS) == false)
   {
	   mur[DEVANT] = 1;
   }
   setPosition(X, Y, sens, table, mur); // ou se trouvent les murs ?
}


// Permet de positionner dans le tableau du labyrinthe les murs
static void setPosition(unsigned char X, unsigned char Y, unsigned char sens,
                 char table[][MAZE_MAX_SIZE], unsigned char *Tab_capt) // Tab_capt = Droite/devant/gauche
{
	char cellule = 0;
	char gauche = sens - 1;
	char droite = sens + 1;

    if (sens == 0)
    {
        gauche += 4;
    }
    else if (sens == 3)
    {
        droite -= 4;
    }
    //   N
    //O     E
    //   S
    // devant mur ou pas ?

    cellule = table[Y][X];
    cellule |= (Tab_capt[DEVANT] << sens);
    cellule |= (Tab_capt[GAUCHE] << gauche);
    cellule |= (Tab_capt[DROITE] << droite);
    table[Y][X] = cellule;
    table[Y][X] |= 16;
}


// Envoi sur le port srie l'affichage du labyrinthe
static void display(char tab[][MAZE_MAX_SIZE], int indice, int sens)
{
//    char chaine[4][6] =
//    { "NORD", "EST", "SUD", "OUEST" };
    unsigned char tabaffich[20][20] =
    { "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    ",
      "                    ", "                    " };
    int ii, jj;
    char pos;
    tabaffich[0][19] = 0;

    hal_ui_clear_scr(app_context.ui);
    for (ii = 0; ii < MAZE_MAX_SIZE; ii++)
    {
        for (jj = 0; jj < MAZE_MAX_SIZE; jj++)
        {
            pos = tab[ii][jj];
            if (pos > 0)
            {
                // Front wall
                if ((pos & 0x01) > 0)
                {
                    tabaffich[ii * 2][jj * 2] = 219;
                    tabaffich[ii * 2][jj * 2 + 1] = 219;
                    tabaffich[ii * 2][jj * 2 + 2] = 219;

                    printf("Line(%d, %d, %d, %d)\n",
                           ii * PIX_AFF_HOST, jj * PIX_AFF_HOST,
                           ii * PIX_AFF_HOST, jj * PIX_AFF_HOST + PIX_AFF_HOST);

                    hal_ui_draw_line(app_context.ui,
                                     ii * PIX_AFF, jj * PIX_AFF,
                                     ii * PIX_AFF, jj * PIX_AFF + PIX_AFF);
                }
                // Right wall
                if ((pos & 0x02) > 0)
                {
                    tabaffich[ii * 2][jj * 2 + 2] = 219;
                    tabaffich[ii * 2 + 1][jj * 2 + 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 2] = 219;

                    printf("Line(%d, %d, %d, %d)\n",
                            ii * PIX_AFF_HOST,
                            jj * PIX_AFF_HOST + PIX_AFF_HOST,
                            ii * PIX_AFF_HOST + PIX_AFF_HOST,
                            jj * PIX_AFF_HOST + PIX_AFF_HOST);

                    hal_ui_draw_line(app_context.ui,
                                     ii * PIX_AFF, jj * PIX_AFF + PIX_AFF,
                                     ii * PIX_AFF + PIX_AFF, jj * PIX_AFF + PIX_AFF);
                }
                // Left wall
                if ((pos & 0x04) > 0)
                {
                    tabaffich[ii * 2 + 2][jj * 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 1] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 2] = 219;

                    printf("Line(%d, %d, %d, %d)\n",
                            ii * PIX_AFF_HOST + PIX_AFF_HOST,
                            jj * PIX_AFF_HOST, ii * PIX_AFF_HOST + PIX_AFF_HOST,
                            jj * PIX_AFF_HOST + PIX_AFF_HOST);

                    hal_ui_draw_line(app_context.ui,
                                     ii * PIX_AFF + PIX_AFF,
                                     jj * PIX_AFF, ii * PIX_AFF + PIX_AFF,
                                     jj * PIX_AFF + PIX_AFF);
                }
                // Rear wall
                if ((pos & 0x08) > 0)
                {
                    tabaffich[ii * 2][jj * 2] = 219;
                    tabaffich[ii * 2 + 1][jj * 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2] = 219;

                    printf("Line(%d, %d, %d, %d)\n",
                            ii * PIX_AFF_HOST, jj * PIX_AFF_HOST,
                            ii * PIX_AFF_HOST + PIX_AFF_HOST, jj * PIX_AFF_HOST);

                    hal_ui_draw_line(app_context.ui,
                                     ii * PIX_AFF, jj * PIX_AFF,
                                     ii * PIX_AFF + PIX_AFF, jj * PIX_AFF);
                }
                if ((pos & 0x10) > 0)
                {
                    if (tabaffich[ii * 2 + 1][jj * 2 + 1] == 35)
                    {
                        tabaffich[ii * 2 + 1][jj * 2 + 1] = 36;

                        printf("Rect(%d, %d, %d, %d)\n",
                                (ii * PIX_AFF_HOST) + (PIX_AFF_HOST / 2),
                                (jj * PIX_AFF_HOST) + (PIX_AFF_HOST / 2),
                                PIX_AFF_HOST / 2, PIX_AFF_HOST / 2);

                        hal_ui_fill_rect(app_context.ui,
                                         (ii * PIX_AFF) + (PIX_AFF / 2),
                                         (jj * PIX_AFF) + (PIX_AFF / 2),
                                         PIX_AFF / 2, PIX_AFF / 2);
                    }
                    else
                    {
                        tabaffich[ii * 2 + 1][jj * 2 + 1] = 35;

                        printf("Circle(%d, %d, %d)\n",
                                (ii * PIX_AFF_HOST) + (PIX_AFF_HOST / 2),
                                (jj * PIX_AFF_HOST) + (PIX_AFF_HOST / 2), 1);

                        hal_ui_fill_circle(app_context.ui,
                                           (ii * PIX_AFF) + (PIX_AFF / 2),
                                           (jj * PIX_AFF) + (PIX_AFF / 2), 1);
                    }
                }
            }
        }
    }
    hal_ui_refresh(app_context.ui);
//    for (ii = 0; ii < indice; ii++)
//    {
//        hal_serial_write(serial, tabaffich[ii],19);
//        if (ii == 5)
//        {
//            hal_serial_write(serial,chaine[sens],strlen(chaine[sens]));
//        }
//        hal_serial_write(serial, "\n",1);
//    }
}




static int robot_calibration(char table[][MAZE_MAX_SIZE],
                             unsigned char x, unsigned char y,
                             unsigned int *sens, bool setPos)
{
    robot_settings  backup_settings;
    unsigned char   sensors_state;
    unsigned int    backup_sens = *sens;

    // Save current settings
    memcpy(&backup_settings, &zhonx_settings, sizeof(robot_settings));

    if (zhonx_settings.calibration_enabled == false)
    {
        /* Calibration is disabled */

        if (setPos == true)
        {
            zhonx_settings.initial_speed = 3000;
            zhonx_settings.max_speed_distance = 20;

            getPosition(x, y, *sens, table);
            display(table, 20, *sens);
            commande(1, 0);

            getPosition(x, y, EST, table);
            display(table, 20, EST);
            commande( -1, 0);
            display(table, 20, *sens);

            // Restore previous settings
            memcpy(&zhonx_settings, &backup_settings, sizeof(robot_settings));
        }

        return 0;
    }

    zhonx_settings.initial_speed = 1500;
    zhonx_settings.max_speed_distance = 1;

    hal_ui_clear_scr(app_context.ui);

    if (setPos == true)
    {
        getPosition(x, y, *sens, table);
        display(table, 20, *sens);
    }

    sensors_state = hal_sensor_get_state(app_context.sensors);

    // Front sensor
    if (check_bit(sensors_state, SENSOR_F10_POS) == false)
    {
        calibrate(NORD, setPos, table, x, y, sens);
    }
    // Right sensor
    else if (check_bit(sensors_state, SENSOR_R10_POS) == false)
    {
        calibrate(EST, setPos, table, x, y, sens);
    }
    // Left sensor
    else if (check_bit(sensors_state, SENSOR_L10_POS) == false)
    {
        calibrate(OUEST, setPos, table, x, y, sens);
    }
    else
    {
        // A calibration is not possible
        return -1;
    }

    // Restore previous settings
    memcpy(&zhonx_settings, &backup_settings, sizeof(robot_settings));
    *sens = backup_sens;

    return 0;
}


static int calibrate(int direction, bool setPos, char table[][MAZE_MAX_SIZE],
                     unsigned char x, unsigned char y, unsigned int *sens)
{
    static int      angle = 0;
    static int      calibrated = 0;
    unsigned char   sensors_state = 0;

    if (calibrated == 2)
    {
        calibrated = 0;
        if (angle != 0)
        {
            commande(-angle, 0);
            angle = 0;
        }
        return 0;
    }

    switch(direction)
    {
        case NORD:
            sensors_state = hal_sensor_get_state(app_context.sensors);
            step_motors_basic_move(70);
            hal_os_sleep(500);
            step_motors_basic_move(-(168 / 4));
            calibrated++;
            break;

        case EST:
            commande(1, 0);
            sensors_state = hal_sensor_get_state(app_context.sensors);
            angle++;
            *sens = get_abs_position(angle);
            if ((calibrated == 1) && (setPos == true))
            {
                getPosition(x, y, *sens, table);
                display(table, 20, *sens);
            }
            step_motors_basic_move(70);
            hal_os_sleep(500);
            step_motors_basic_move(-(168 / 4));
            calibrated++;
            break;

        case OUEST:
            commande(-1, 0);
            sensors_state = hal_sensor_get_state(app_context.sensors);
            angle--;
            *sens = get_abs_position(angle);
            if ((calibrated == 1) && (setPos == true))
            {
                getPosition(x, y, *sens, table);
                display(table, 20, *sens);
            }
            step_motors_basic_move(70);
            hal_os_sleep(500);
            step_motors_basic_move(-(168 / 4));
            calibrated++;
            break;
    }

    if (check_bit(sensors_state, SENSOR_R10_POS) == false)
    {
        calibrate(EST, setPos, table, x, y, sens);
    }
    else if (check_bit(sensors_state, SENSOR_L10_POS) == false)
    {
        calibrate(OUEST, setPos, table, x, y, sens);
    }

    return 0;
}


static unsigned char get_abs_position(int angle)
{
    angle %= 4;

    switch (angle)
    {
        case 1:
        case -3:
            return EST;
        case 3:
        case -1:
            return OUEST;
        default:
            if (angle < 0)
            {
                angle = -angle;
            }
            return angle;
    }
}
static void trajectoire2(char *Tab, int taille)
{
	int ii = 1;
	int dist = 0;
	int distance = 0;
	int tourne = 0;
	double angle = 0;

	step_motors_move(90, 0, CHAIN_AFTER);

	while (ii < (taille - 1))
	{
		ii++;
		tourne = (Tab[ii] - Tab[ii - 1]);

		dist = 0;

		//--------------------------------
		if (tourne == 0)
		{
			dist = 1;
			while (Tab[ii + 1] == Tab[ii] && ii < (taille - 1))
			{
				dist++;
				ii++;
			}

			distance=dist*180;
			step_motors_move(distance, 0, CHAIN_BEFORE | CHAIN_AFTER);

		}
		else
		{
            if (tourne == 3)
            {
                tourne = -1;
            }
            else if (tourne == -3)
            {
                tourne=1;
            }
			angle = tourne * 90.0;
			step_motors_rotate(angle, 90, 0);
		}
	}
	step_motors_move(90, 0, 0);
}


static void trajectoire3(char *Tab, int taille)
{
	int ii = 1;
	int dist = 0;
	int distance = 0;
	int tourne = 0;
	double angle = 0;

	//zhonx_settings.max_speed_distance = (250 * STEPS_PER_MM);

	if (taille > 2)
	{
		if (Tab[2] - Tab[1] == 0)
		{
			ii = 2;
			step_motors_move(90+180, 0, CHAIN_AFTER);
		}
		else
		{
			step_motors_move(90, 0, CHAIN_AFTER);
		}
	}
	else
	{
		step_motors_move(90, 0, CHAIN_AFTER);
	}

	while (ii<taille-1)
	{
		ii++;
		tourne = (Tab[ii] - Tab[ii - 1]);

		dist = 0;

		//--------------------------------
		if (tourne == 0)
		{
			dist = 1;
			while ((Tab[ii + 1] == Tab[ii]) && (ii < (taille-1)))
			{
                dist++;
                ii++;
			}

			distance = dist * 180;
			step_motors_move(distance, 0, CHAIN_BEFORE | CHAIN_AFTER);
		}
		else
		{
			if (tourne == 3)
            {
                tourne = -1;
            }
            else if (tourne == -3)
            {
                tourne = 1;
            }
            angle = tourne * 90.0;
			step_motors_rotate(angle, 90, 0);
		}
	}
	step_motors_move(90, 0, CHAIN_BEFORE);
}
void commande(int dir, int dist)
{
    double angle;
    int distance;

	if (dir == 3)
	{
	   dir = -1;
	}
	else if (dir == -3)
	{
	    dir = 1;
	}

	angle = dir * 90.0;
    distance = dist * CELL_LENGTH;

    if (dist > 0)
    {
    	step_motors_rotate_in_place(-angle);
    	step_motors_move(distance, 0, 0);
    }
    else
    {
    	step_motors_move(distance, 0, 0);
    	step_motors_rotate_in_place(-angle);
    }
}
void trajectoire1(char *Tab, int taille)
{
	int ii=1;
	int dist=0;
	int tourne=0;
	char tmp_tab[(MAZE_MAX_SIZE * MAZE_MAX_SIZE) / 4];

	memcpy(tmp_tab, Tab, taille);
	tmp_tab[0] = tmp_tab[1];

	while (ii<taille)
	{
	    //ii++;
		tourne = (tmp_tab[ii]-tmp_tab[ii-1]);
		if (tourne > 3) tourne = tourne - 4;
		if (tourne < 0) tourne = tourne + 4;
		dist=1; ii++;
		while (tmp_tab[ii]==tmp_tab[ii-1] && ii<taille)
		{
			dist++; ii++;
		}
		commande(tourne, dist);
	}
}
