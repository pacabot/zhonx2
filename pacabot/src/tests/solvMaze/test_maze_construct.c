#if 0

#include <string.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "hal/hal_serial.h"

#include "config/config.h"
#include "util/common_functions.h"

// PB10 -> Droite
// PB11 -> gauche

#define DROITE 0
#define GAUCHE 1
#define DEVANT 2
#define NORD  0
#define EST   1
#define SUD   2
#define OUEST 3

// liste des fonctions définies dans le programme.
void display(char tab[][MAZE_MAX_SIZE], int indice, int sens);
void getPosition(unsigned char X, unsigned char Y, unsigned char sens,
                 char table[][MAZE_MAX_SIZE]);
void setPosition(unsigned char X, unsigned char Y, unsigned char sens,
                 char table[][MAZE_MAX_SIZE], unsigned char *Tab_capt);
void commande(int dir, int dist);
void trajectoire1(char * Tab, int taille);
void trajectoire2(char * Tab, int taille);
void trajectoire3(char * Tab, int taille);

int Avancer2(char table[][MAZE_MAX_SIZE], unsigned int ligne,
             unsigned int colonne, unsigned int sens);

int trouve2 = 0;
char _porte2[] =
{1, 2, 4, 8};
int dx2[] =
{0, 1, 0, -1};
int dy2[] =
{-1, 0, 1, 0};
char tabsens[MAZE_MAX_SIZE * MAZE_MAX_SIZE / 4];
int increment = 0;

HAL_SERIAL_HANDLE handle;

void init(void);

// Envoi sur le port série l'affichage du labyrinthe
void display2(char tab[][MAZE_MAX_SIZE], int indice, int sens)
{
    char chaine[4][6] =
    {"NORD", "EST", "SUD", "OUEST"};
    unsigned char tabaffich[20][20] =
    {"                    ", "                    ", "                    ",
     "                    ", "                    ", "                    ",
     "                    ", "                    ", "                    ",
     "                    ", "                    ", "                    ",
     "                    ", "                    ", "                    ",
     "                    ", "                    ", "                    ",
     "                    ", "                    "};
    int ii, jj;
    char pos;
    tabaffich[0][19] = 0;

    for (ii = 0; ii < MAZE_MAX_SIZE; ii++)
    {
        for (jj = 0; jj < MAZE_MAX_SIZE; jj++)
        {
            pos = tab[ii][jj];
            if (pos > 0)
            {
                if ((pos & 1) > 0)
                {
                    tabaffich[ii * 2][jj * 2] = 219;
                    tabaffich[ii * 2][jj * 2 + 1] = 219;
                    tabaffich[ii * 2][jj * 2 + 2] = 219;
                }
                if ((pos & 2) > 0)
                {
                    tabaffich[ii * 2][jj * 2 + 2] = 219;
                    tabaffich[ii * 2 + 1][jj * 2 + 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 2] = 219;
                }
                if ((pos & 4) > 0)
                {
                    tabaffich[ii * 2 + 2][jj * 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 1] = 219;
                    tabaffich[ii * 2 + 2][jj * 2 + 2] = 219;
                }
                if ((pos & 8) > 0)
                {
                    tabaffich[ii * 2][jj * 2] = 219;
                    tabaffich[ii * 2 + 1][jj * 2] = 219;
                    tabaffich[ii * 2 + 2][jj * 2] = 219;
                }
                if ((pos & 16) > 0)
                {
                    if (tabaffich[ii * 2 + 1][jj * 2 + 1] == 35)
                    {
                        tabaffich[ii * 2 + 1][jj * 2 + 1] = 36;
                    }
                    else
                    {
                        tabaffich[ii * 2 + 1][jj * 2 + 1] = 35;
                    }
                }
            }
        }
    }

    for (ii = 0; ii < indice; ii++)
    {
        hal_serial_write(handle, tabaffich[ii], 19);
        if (ii == 5)
        {
            hal_serial_write(handle, chaine[sens], strlen(chaine[sens]));
        }
        hal_serial_write(handle, "\n", 1);
        //printf("%s", tab[i]);
    }
    for (ii = 0; ii < 0x50000; ii++)
        ;
}

void SetPosition2(unsigned char X, unsigned char Y, unsigned char sens,
                  char table[][MAZE_MAX_SIZE], unsigned char *Tab_capt) // Tab_capt = Droite/devant/gauche
{
    char cellule = 0;
    // gauche mur ou pas ?
    char gauche = sens - 1;
    char droite;

    if (sens == 0)
    {
        gauche = gauche + 4;
    }

    droite = sens + 1;
    if (sens == 3)
    {
        droite -= 4;
    }
    //   N
    //O     E
    //   S
    // devant mur ou pas ?

    cellule = table[Y][X];
    cellule = cellule | (Tab_capt[DEVANT] << sens);
    cellule = cellule | (Tab_capt[GAUCHE] << gauche);
    cellule = cellule | (Tab_capt[DROITE] << droite);
    table[Y][X] = cellule;
    table[Y][X] = table[Y][X] | 16;
}

void capteurs2(unsigned char *Tab_capt)
{

    Tab_capt[DROITE] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
    Tab_capt[GAUCHE] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
    Tab_capt[DEVANT] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);

    if (Tab_capt[DROITE] == 1)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    }
    if (Tab_capt[GAUCHE] == 1)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
    }
}

void GetPosition2(unsigned char X, unsigned char Y, unsigned char sens,
                  char table[][MAZE_MAX_SIZE])
{
    unsigned char mur[3];
//   display2(table, 11,sens);
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
    {
        capteurs2(mur);
    }
    while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
    {
        capteurs2(mur);
    }
    SetPosition2(X, Y, sens, table, mur); // ou se trouve les murs ?
}

int test_maze_construc(void)
{
    char tableau[MAZE_MAX_SIZE][MAZE_MAX_SIZE] =
    {("0000000000"), ("0000000000"), ("0000000000"), ("0000000000"),
     ("0000955530"), ("0000A95560"), ("0000AA9150"), ("0000AEAAB0"),
     ("0000C56C60")};
    /*
     {("9555300000"),
     ("C395600000"),
     ("96C1300000"),
     ("C3FAC00000"),
     ("D454700000"),
     ("0000000000")};
     */
    /*
     {("0000955530"),
     ("00008393A0"),
     ("0000EC6AA0"),
     ("0000B93A80"),
     ("0000C6C6E0"),
     ("0000000000")};
     */
    /*
     {("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000B93F90"),
     ("0000AAC160"),
     ("0000AAD2B0"),
     ("0000AA9460"),
     ("0000C6C570"),
     ("0000000000")};

     {
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000951530"),
     ("0000A943E0"),
     ("000082F810"),
     ("0000AC16A0"),
     ("0000ED4560"),
     ("0000000000")};

     {
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000D53930"),
     ("0000916AA0"),
     ("0000E87AA0"),
     ("0000943AA0"),
     ("00006FC6E0"),
     ("0000000000")};
     */
    /*
     {("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000"),
     ("0000000000")};
     */

    ConvertBinaire(tableau);

    //  unsigned char mur[3];
    //  unsigned char i;
    unsigned char x = 4;
    unsigned char y = 4;
    unsigned int sens = NORD;

    init();
    hal_serial_write(handle, "Commencement du test\n", 23);
    display2(tableau, 20, sens);
    hal_serial_write(handle, "Rotation 180\n", 13);
    GetPosition2(x, y, sens, tableau);
    //commande( SUD, 0);
    display2(tableau, 20, SUD);
    int ret = 0;
    ret = Avancer2(tableau, x, y, SUD);
    hal_serial_write(handle, "Le test est termine\n", 20);
    Convertcharacter(tabsens, ret + 1);
    hal_serial_write(handle, "trajectoire a envoyer\n", 22);
    hal_serial_write(handle, tabsens, ret + 1);

    return ret + 1;
}

int Avancer2(char table[][MAZE_MAX_SIZE], unsigned int X, unsigned int Y,
             unsigned int sens)
{
    int i;
    int tourner = 0;
    int avancer = 0;

    table[Y][X] = table[Y][X] | 16;
    // Interrogation des capteurs pour déterminer si il y a un mur devant/droite/gauche
    // GetPosition(X, Y, sens, table);
    display2(table, 20, sens);
    // est-on sur la case de fin
    if (table[Y][X] == 0x10)
    {
        tabsens[increment + 1] = sens;
        return (increment + 1); // On est en dehors de l'initialisation du tableau donc hors du labyrinthe
    }

    if (!trouve2)
    {          // On n'a pas encore trouvé l'arrivée
        char porte;
        unsigned char tmpX, tmpY;

        for (i = 0; i <= 3; i++)
        {
            if (!trouve2)
            {
                porte = (table[Y][X] & _porte2[i]);  // Test N, E, S, W
                if (porte == 0)
                { //0= pas de mur!
                    tmpX = (X + dx2[i]);      // Calcul de la prochaine position
                    tmpY = (Y + dy2[i]);
                    porte = (table[tmpY][tmpX]) & 0x10; // Pour tester si on est déjà passé par là
                    if (porte == 0)
                    {                // c'est bon on est pas encore passé par là
                        increment++;
                        tabsens[increment] = i; // Enregistrer la nouvelle position

                        tourner = tabsens[increment] - tabsens[increment - 1]; // calcul de la rotation à envoyer
                        avancer = 1;
                        //commande( tourner,avancer);
                        trouve2 = Avancer2(table, tmpX, tmpY, i);
                    }
                }
            }
        }
        // Si on ne veut pas que le robot retourne au depart tout en marche arriere depuis l'arrivee
        tourner = tabsens[increment] - tabsens[increment - 1];
        increment--;
        avancer = 1;
        //    table[Y][X] = table[Y][X]-16; // on efface la trace de notre passage
        commande(tourner, avancer);
    }

    return trouve2;
}

void init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char serial_port = 0;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12
                                  | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    hal_serial_init();
    hal_serial_open(&(handle), &serial_port, 115200);
}

#endif
