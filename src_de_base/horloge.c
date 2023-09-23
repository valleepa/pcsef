#include <inttypes.h>
#include <stdio.h>
#include "horloge.h"
#include "ecran.h"
#include "cpu.h"
#include "segment.h"

uint8_t nb_tics = 0;
uint32_t nb_secondes = 0;
char str[LONGUEUR_AFFICHAGE];
/*
Traitant de l'interruption de l'horloge (appelé à chaque interruption PIT)
Commence par une partie assembleur dans traitant.S pour sauvegarder les registres
*/
void tic_PIT(void)
{
    outb(0x20, 0x20); // acquittement de l'interruption
    nb_tics++;
    if(nb_tics == CLOCKFREQ)
    {
        nb_secondes += 1;
        sprintf(str, "%02uh%02um%02us", nb_secondes / 3600, (nb_secondes / 60) % 3600, nb_secondes % 60);
        ecrit_temps(str, LONGUEUR_AFFICHAGE);
        nb_tics = 0;
    }
}

/*
Prend un numéro d'interruption et la fonction assembleur traitant_IT_32 définie dans traitant.S
Initialise la table des vecteurs d'interruption (IDT).
*/
void init_traitant_IT(uint32_t num_IT, void (*traitant) (void))
{
    uint32_t * table = (uint32_t *) 0x1000;
    table += 2 * num_IT;

    uint32_t first_word = KERNEL_CS << 16;
    first_word |= (((uint32_t) traitant) & 0xFFFF);
    uint32_t last_word = (((uint32_t) traitant) & 0xFFFF0000);
    last_word |= 0x8E00;

    *table = first_word;
    *(table + 1) = last_word;
}

/*
Ecrit la chaine de caractères en haut à droite de l'écran
*/
void ecrit_temps(char * s, uint8_t taille)
{
    place_curseur(0, NB_COL - taille);
    console_putbytes(s, taille);
}

/*
Prend le numéro de l'IRQ (0..7) à gérer ainsi qu'un booléen indiquant si on souhaite masquer (1) ou démasquer (0)
*/
void masque_IRQ(uint32_t num_IRQ, bool masque)
{
    uint8_t tab_masques = inb(0x21);
    if(masque)
    {
        tab_masques |= 1 << num_IRQ;
    }
    else
    {
        uint8_t zeroes = 0x0;
        zeroes |= 1 << num_IRQ; // isolement du bit irq à 1
        tab_masques &= !zeroes;
    }

    outb(tab_masques, 0x21);
}

/*
It's all in the title
*/
void setup_clock_frequency(uint16_t clock_freq)
{
    outb(0x34, 0x43); // indique à l'horloge que l'on va lui envoyer la valeur de réglage de la fréquence 
    outb((QUARTZ / clock_freq) & 0xFF, 0x40); // low
    outb(((QUARTZ / clock_freq) & 0xFF00) >> 8, 0x40); // high
}