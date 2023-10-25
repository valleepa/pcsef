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
    struct Processus * suiv;
} Processus;

void idle(void);
void fct_proc_i(void);
void init_process_list();
void ordonnance();
char * mon_nom();
uint8_t mon_pid();
/*
prend en
paramètre le code de la fonction à exécuter (ainsi que le nom du processus) et renvoie le pid du
processus créé, ou -1 en cas d’erreur (si on a essayé de créer plus de processus que le nombre
maximum)
*/
uint8_t cree_processus(void (*code)(void), char * nom);
/*
Extrait la tête de la LC des activables et la renvoie
*/
Processus * extraction_tete_activables();

/*
Extrait la tête pour la mettre en queue. Pour mettre idle en fin de liste et ainsi ne pas le répeter au début
*/
void tete_a_queue();

/*
Ajoute le processus proc en fin de LC des activables
*/
void insertion_queue_activable(Processus * proc);

#endif