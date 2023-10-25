#include <stdio.h>
#include <processus.h>
#include <string.h>
#include <cpu.h>
#include <tinyalloc.h>

Processus * proc_actif;
Processus * tete_liste;
Processus * queue_liste;
uint8_t nb_processus_crees;

void idle(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}

void fct_proc_i(void)
{
    for(;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}

void ordonnance()
{
    Processus * ancien_elu = proc_actif;
    Processus * nouveau_elu = extraction_tete_activables();
    insertion_queue_activable(proc_actif);
    proc_actif = nouveau_elu;
    ctx_sw((uint32_t *) &(ancien_elu->registres), (uint32_t *) &(nouveau_elu->registres));
}

char * mon_nom()
{
    return proc_actif->nom;
}

uint8_t mon_pid()
{
    return proc_actif->pid;
}

void init_process_list()
{
    Processus * idle = (Processus *) malloc(sizeof(Processus));
    strcpy(idle->nom, "idle");
    idle->etat = Elu;
    idle->pid = 0;
    proc_actif = idle;
    tete_liste = idle;
    queue_liste = idle;
    nb_processus_crees = 1;
}


void insertion_queue_activable(Processus * proc)
{
    queue_liste->suiv = proc;
    queue_liste = proc;
    queue_liste->etat = Activable;
}


Processus * extraction_tete_activables()
{
    Processus * proc = tete_liste;
    tete_liste = tete_liste->suiv;
    proc->suiv = NULL;
    proc->etat = Elu;
    return proc;
}

void tete_a_queue()
{
    extraction_tete_activables();
    insertion_queue_activable(proc_actif);
}

uint8_t cree_processus(void (*code)(void), char * nom)
{
    Processus * proc = (Processus *) malloc(sizeof(Processus));
    queue_liste->suiv = proc;
    queue_liste = proc;

    strcpy(proc->nom, nom);
    proc->etat = Activable;
    proc->registres[ESP] = (uint32_t) &(proc->pile[TAILLE_PILE - 1]);
    proc->pile[TAILLE_PILE - 1] = (uint32_t) code;
    proc->pid = nb_processus_crees;
    nb_processus_crees++;

    return proc->pid;
}