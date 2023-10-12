#include <stdio.h>
#include <processus.h>
#include <string.h>
#include <cpu.h>
#include <tinyalloc.h>

Processus * proc_actif;
Processus ** procs;
uint8_t index_table_utilise;

void idle(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}

void fct_proc_i(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}

void ordonnance()
{
    uint8_t pid_proc_a_elire = (proc_actif->pid + 1) % NB_PROCESSUS;
    Processus * nouveau_elu = get_processus(pid_proc_a_elire);

    nouveau_elu->etat = Elu;
    proc_actif->etat = Activable;
    Processus * ancien_elu = proc_actif;
    proc_actif = nouveau_elu;
    ctx_sw((uint32_t *) &(ancien_elu->registres), (uint32_t *) &(nouveau_elu->registres));
}

Processus * get_processus(uint8_t pid)
{
    Processus * res;
    for(uint8_t i = 0; i < NB_PROCESSUS; i++)
    {
        if(procs[i] != NULL && procs[i]->pid == pid)
        {
            res = procs[i];
        }
    }

    return res;
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
    index_table_utilise = 0;
    procs = (Processus **) malloc(NB_PROCESSUS * sizeof(Processus *));
    for(uint8_t i = 0; i < NB_PROCESSUS; i++)
    {
        procs[i] = (Processus *) malloc(sizeof(Processus));
    }
}

void set_proc_actif(uint8_t pid)
{
    proc_actif = get_processus(pid);
}

uint8_t cree_processus(void (*code)(void), char * nom)
{
    if(index_table_utilise < NB_PROCESSUS)
    {
        Processus * proc = procs[index_table_utilise];
        strcpy(proc->nom, nom);
        proc->pid = index_table_utilise;
        if(proc->pid == 0) // idle
        {
            proc->etat = Elu;
        }
        else
        {
            proc->etat = Activable;
            proc->registres[ESP] = (uint32_t) &(proc->pile[TAILLE_PILE - 1]);
            proc->pile[TAILLE_PILE - 1] = (uint32_t) code;    
        }

        index_table_utilise++;
    }
    return -1;
}