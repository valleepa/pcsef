
#ifndef __ECRAN_H__
#define __ECRAN_H__
#define MEM_START 0xB8000
#define NB_LIG 25
#define NB_COL 80
#define SIG_LOW 0x0F
#define SIG_HIGH 0x0E
#define COMMAND_PORT 0x3D4
#define DATA_PORT 0x3D5
#define BS 8
#define HT 9
#define LF 10
#define FF 12
#define CR 13
#define BLANC 15
#define NOIR 0

#include<cpu.h>
#include<string.h>

/*
renvoie un pointeur sur la
case mémoire correspondant aux coordonnées fournies 
*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col);

/*
écrit le caractère c
aux coordonnées spécifiée
*/
void ecrit_car(uint32_t lig, uint32_t col, char c);

/*
doit parcourir les lignes et les colonnes de l’écran
pour écrire dans chaque case un espace en blanc sur fond noir (afin d’initialiser les formats dans
la mémoire)
*/
void efface_ecran(void);

/*
place le curseur à la po-
sition donnée
*/
void place_curseur(uint32_t lig, uint32_t col);

/*
traite un caractère donné (c’est à dire qui l’affiche si
c’est un caractère normal ou qui implante l’effet voulu si c’est un caractère de contrôle) 
*/
void traite_car(char c);

/*
fait remonter d’une ligne l’affichage à l’écran (il pourra
être judicieux d’utiliser memmove définie dans string.h pour cela)
*/
void defilement(void);

/*
met à jour la position du curseur.
*/
void update_position_curseur();

void console_putbytes(const char *s, int len);

#endif
