#include <stdio.h>
#include <processus.h>
#include <string.h>
#include <cpu.h>

Processus procs[NB_PROCESSUS];
Processus * proc_actif;

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
    if(procs[0].etat == Elu)
    {
        procs[0].etat = Activable;
        procs[1].etat = Elu;
        proc_actif = &procs[1];
        ctx_sw((uint32_t *) &procs[0].registres, (uint32_t *) &procs[1].registres);
    }
    else
    {
        procs[0].etat = Elu;
        procs[1].etat = Activable;
        proc_actif = &procs[0];
        ctx_sw((uint32_t *) &procs[1].registres, (uint32_t *) &procs[0].registres);
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
    strcpy(procs[0].nom, "idle");
    strcpy(procs[1].nom, "proc1");
    procs[0].pid = 0;
    procs[1].pid = 1;
    procs[0].etat = Elu;
    procs[1].etat = Activable;
    proc_actif = &procs[0];

    procs[1].registres[ESP] = (uint32_t) &(procs[1].pile[TAILLE_PILE - 1]);
    procs[1].pile[TAILLE_PILE - 1] = (uint32_t) &proc1;
}