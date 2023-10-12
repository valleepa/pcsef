#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__
#define TAILLE_NOM 20
#define NB_REGISTRES 5
#define TAILLE_PILE 512
#define NB_PROCESSUS 8
#define ESP 1
#include <inttypes.h>

typedef enum Etat {Elu = 1, Activable = 0} Etat;

extern void ctx_sw(uint32_t *, uint32_t *);

typedef struct Processus
{
    uint8_t pid;
    char nom[TAILLE_NOM];
    Etat etat;
    uint32_t registres[NB_REGISTRES];
    uint32_t pile[TAILLE_PILE];
} Processus;

extern Processus ** procs;

void idle(void);
void fct_proc_i(void);
void make_process_table();
void ordonnance();
char * mon_nom();
uint8_t mon_pid();
Processus * get_processus(uint8_t pid);
void set_proc_actif(uint8_t pid);
/*
prend en
paramètre le code de la fonction à exécuter (ainsi que le nom du processus) et renvoie le pid du
processus créé, ou -1 en cas d’erreur (si on a essayé de créer plus de processus que le nombre
maximum)
*/
uint8_t cree_processus(void (*code)(void), char * nom);

#endif