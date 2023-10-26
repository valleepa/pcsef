#include <cpu.h>
#include <stdio.h>
#include <inttypes.h>
#include <ecran.h>
#include <horloge.h>
#include <processus.h>

// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }

    printf("fact : %d", res);
    printf("\n");
    printf("\t");

    return res;
}

void kernel_start(void)
{
    //initialisations
    efface_ecran();
    init_traitant_IT(32, traitant_IT_32);
    setup_clock_frequency(CLOCKFREQ);
    masque_IRQ(0, 0); // démasque la clock

    cree_processus(&idle, "idle");
    cree_processus(&proc1, "proc 1");
    cree_processus(&proc2, "proc 2");
    cree_processus(&proc3, "proc 3");
    
    idle();

    // démasquage des interruptions externes
    // sti();
    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}

