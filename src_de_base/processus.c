#include <stdio.h>
#include <processus.h>
#include <string.h>
#include <cpu.h>
#include <tinyalloc.h>
#include <horloge.h>

Processus * proc_actif;
Processus * tete_liste;
Processus * queue_liste;
Processus * tete_liste_endormis;
Processus * queue_liste_endormis;
uint8_t nb_processus_endormis = 0;
uint8_t nb_processus_activables = 0;
uint8_t nb_processus_crees;

void idle(void)
{
    for(;;)
    {
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for(;;)
    {
        printf("[temps = %u] processus %s pid = %i\n", get_temps(), mon_nom(), mon_pid());
        dors(2);
    }
}

void proc2(void)
{
    for(;;)
    {
        printf("[temps = %u] processus %s pid = %i\n", get_temps(), mon_nom(), mon_pid());
        dors(3);
    }
}

void proc3(void)
{
    for(;;)
    {
        printf("[temps = %u] processus %s pid = %i\n", get_temps(), mon_nom(), mon_pid());
        dors(5);
    }
}

void ordonnance()
{
    Processus * ancien_elu;
    Processus * nouvel_elu;

    ancien_elu = proc_actif;
    
    if(nb_processus_endormis > 0)
    {
        reveiller_processus_endormis();       
    }
    if(nb_processus_activables > 0)
    {
        nouvel_elu = extraction_tete_activables();
        insertion_queue_activable(proc_actif);
        proc_actif = nouvel_elu;
        ctx_sw((uint32_t *) &(ancien_elu->registres), (uint32_t *) &(nouvel_elu->registres));
    }
}

void reveiller_processus_endormis()
{
    uint32_t heure = get_temps();
    Processus * proc = tete_liste_endormis;
    while(proc != NULL)
    {
        if(proc->heure_reveil > heure)
        {
            break;
        }
        else
        {
            Processus * nouveau_proc = extraction_tete_endormis();
            insertion_queue_activable(nouveau_proc);
            nb_processus_activables++;
            nb_processus_endormis--;
        }
        proc = proc->suiv;
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

void insertion_endormis(Processus * proc)
{
    if(nb_processus_endormis == 0)
    {
        tete_liste_endormis = proc;
        queue_liste_endormis = proc;
    }
    else
    {
        Processus * precedent = tete_liste_endormis;
        Processus * courant = precedent->suiv;
        if(proc->heure_reveil < precedent->heure_reveil)
        {
            tete_liste_endormis = proc;
            proc->suiv = precedent;
        }
        else
        {
            while(courant != NULL && proc->heure_reveil >= courant->heure_reveil)
            {
                courant = courant->suiv;
                precedent = precedent->suiv;
            }

            precedent->suiv = proc;
            if(courant != NULL)
            {
                proc->suiv = courant;
            }
            else // fin de liste
            {
                queue_liste_endormis = proc;
            }
        }
    }
    nb_processus_activables--;
    nb_processus_endormis++;
}


Processus * extraction_tete_activables()
{
    Processus * proc = tete_liste;
    tete_liste = tete_liste->suiv;
    proc->suiv = NULL;
    proc->etat = Elu;
    return proc;
}

Processus * extraction_tete_endormis()
{
    Processus * proc = tete_liste_endormis;
    tete_liste_endormis = tete_liste_endormis->suiv;
    proc->suiv = NULL;
    proc->etat = Activable;
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
    nb_processus_activables++;

    return proc->pid;
}

void dors(uint32_t nbr_secs)
{
    Processus * proc_endormi = proc_actif;
    proc_actif->etat = Endormi;
    proc_actif->heure_reveil = get_temps() + nbr_secs;
    insertion_endormis(proc_actif);
    proc_actif = extraction_tete_activables();
    ctx_sw((uint32_t *) &(proc_endormi->registres), (uint32_t *) &(proc_actif->registres));
}