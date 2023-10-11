#include <stdio.h>
#include <processus.h>
#include <string.h>
#include <cpu.h>
#include <tinyalloc.h>

Processus * proc_actif;
Processus ** procs;

void idle(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}

void proc1(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}

void ordonnance()
{
    Processus * idle = procs[0];
    Processus * proc1 = procs[1];
    if(idle->etat == Elu)
    {
        idle->etat = Activable;
        proc1->etat = Elu;
        proc_actif = proc1;
        ctx_sw((uint32_t *) &(idle->registres), (uint32_t *) &(proc1->registres));
    }
    else
    {
        idle->etat = Elu;
        proc1->etat = Activable;
        proc_actif = idle;
        ctx_sw((uint32_t *) &(proc1->registres), (uint32_t *) &(idle->registres));
    }
}

char * mon_nom()
{
    return proc_actif->nom;
}

uint8_t mon_pid()
{
    return proc_actif->pid;
}

void make_process_table()
{
    procs = (Processus **) malloc(NB_PROCESSUS * sizeof(Processus *));
    procs[0] = (Processus *) malloc(sizeof(Processus));
    procs[1] = (Processus *) malloc(sizeof(Processus));
    strcpy(procs[0]->nom, "idle");
    strcpy(procs[1]->nom, "proc1");
    procs[0]->pid = 0;
    procs[1]->pid = 1;
    procs[0]->etat = Elu;
    procs[1]->etat = Activable;
    proc_actif = procs[0];

    procs[1]->registres[ESP] = (uint32_t) &(procs[1]->pile[TAILLE_PILE - 1]);
    procs[1]->pile[TAILLE_PILE - 1] = (uint32_t) &proc1;
}