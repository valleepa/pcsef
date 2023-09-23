#ifndef __HORLOGE_H__
#define __HORLOGE_H__
#include <stdbool.h>
#define QUARTZ 0x1234DD
#define CLOCKFREQ 50
#define LONGUEUR_AFFICHAGE 10

void traitant_IT_32(void);

/*
Prend un numéro d'interruption et la fonction assembleur traitant_IT_32 définie dans traitant.S
Initialise la table des vecteurs d'interruption (IDT).
*/
void init_traitant_IT(uint32_t num_IT, void (*traitant) (void));

/*
Ecrit la chaine de caractères en haut à droite de l'écran
*/
void ecrit_temps(char * s, uint8_t taille);

/*
Traitant de l'interruption de l'horloge (appelé à chaque interruption PIT)
Commence par une partie assembleur dans traitant.S pour sauvegarder les registres
*/
void tic_PIT(void);

/*
Prend le numéro de l'IRQ (0..7) à gérer ainsi qu'un booléen indiquant si on souhaite masquer (1) ou démasquer (0)
*/
void masque_IRQ(uint32_t num_IRQ, bool masque);

/*
It's all in the title
*/
void setup_clock_frequency(uint16_t clock_freq);

#endif