/*---------------------------------------------------------------------------
 *
 *      common_functions.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __COMMON_FUNCTIONS_H__
#define __COMMON_FUNCTIONS_H__

/* Exported functions */

// prend une chaine de caractère composée de chiffres Hexa et convertit en binaire
void ConvertBinaire(char tab[][MAZE_MAX_SIZE]);

//une séquence binaire est convertie en chaine pour affichage
void Convertcharacter(char tab[],char nb);

// Envoi sur le port série l'affichage du labyrinthe
//void display(char tab[][MAZE_MAX_SIZE], int indice, int sens);

// Permet de faire 4 bips
void bip4(void);

const char *hexdump(const void *data, unsigned int len);

// Converts a string to upper case
char *touppercase(char *str, unsigned int len);

#endif // __COMMON_FUNCTIONS_H__
