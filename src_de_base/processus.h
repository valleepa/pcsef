#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__
#define TAILLE_NOM 20
#define NB_REGISTRES 5
#define TAILLE_PILE 512
#define NB_PROCESSUS 2
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
void proc1(void);
void make_process_table();
void ordonnance();
char * mon_nom();
uint8_t mon_pid();

#endif