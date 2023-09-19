#include <inttypes.h>
#include <ecran.h>

uint16_t position_curseur_lig = 0;
uint16_t position_curseur_col = 0;
uint8_t couleur = BLANC;
uint8_t couleur_fond = NOIR;
uint8_t clignotement = 0;

/*
renvoie un pointeur sur la
case mémoire correspondant aux coordonnées fournies 
*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col)
{
    return (uint16_t *) (MEM_START + 2 * (lig * NB_COL + col)); // attention aux parenthèses du cast
}

/*
écrit le caractère c
aux coordonnées spécifiée, et met à jour la position du curseur
*/
void ecrit_car(uint32_t lig, uint32_t col, char c)
{
    uint16_t word = 0 | c;
    word |= couleur << 8;
    word |= couleur_fond << 12;
    word |= clignotement << 15;

    uint16_t * addresse_memoire = ptr_mem(lig, col);
    *addresse_memoire = word;

    position_curseur_lig = lig;
    position_curseur_col = col;
    update_position_curseur();

}

/*
met à jour la position du curseur.
*/
void update_position_curseur()
{
    if(position_curseur_col == NB_COL - 1)
    {
        if(position_curseur_lig == NB_LIG - 1)
        {
            position_curseur_col = 0;
            position_curseur_lig = 0;
        }
        position_curseur_lig++;
        position_curseur_col = 0;
    }
    else
    {
        position_curseur_col++;
    }
}

/*
doit parcourir les lignes et les colonnes de l’écran
pour écrire dans chaque case un espace en blanc sur fond noir (afin d’initialiser les formats dans
la mémoire)
*/
void efface_ecran(void)
{
    for(int i = 0; i < NB_LIG; i++)
    {
        for(int j = 0; j < NB_COL; j++)
        {
            couleur = BLANC;
            couleur_fond = NOIR;
            ecrit_car(position_curseur_lig, position_curseur_col, ' ');
        }
    }
}

/*
place le curseur à la po-
sition donnée
*/
void place_curseur(uint32_t lig, uint32_t col)
{
    position_curseur_col = col;
    position_curseur_lig = lig;

    uint16_t position_curseur = col + lig * NB_COL;

    outb(SIG_LOW, COMMAND_PORT);
    outb(position_curseur & 0x00ff, DATA_PORT);
    outb(SIG_HIGH, COMMAND_PORT);
    outb((position_curseur & 0xff00) >> 8, DATA_PORT);
}

/*
traite un caractère donné (c’est à dire qui l’affiche si
c’est un caractère normal ou qui implante l’effet voulu si c’est un caractère de contrôle) 
*/
void traite_car(char c)
{
    if(c > 127) // accent
    {
        return;
    }
    
    if(c <= 31) // caractère de contrôle
    {
        switch (c)
        {
        case BS:
            if (position_curseur_col != 0)
            {
                position_curseur_col--;
            }   
            break;
        case HT:
            if(position_curseur_col == NB_COL - 1)
                break;
            position_curseur_col++;
            while(position_curseur_col % 8 != 0 && position_curseur_col != NB_COL - 1)
            {
                position_curseur_col++;
            }
            break;
        case LF:
            if(position_curseur_lig != NB_LIG - 1)
            {
                position_curseur_lig++;
            }
            break;
        case FF:
            efface_ecran();
            position_curseur_col = 0;
            position_curseur_lig = 0;
            break;
        case CR:
            position_curseur_col = 0;
            break;
        default:
            break;
        }

        place_curseur(position_curseur_lig, position_curseur_col);
    }
    else
    {
        ecrit_car(position_curseur_lig, position_curseur_col, c);
    }
}

/*
fait remonter d’une ligne l’affichage à l’écran (il pourra
être judicieux d’utiliser memmove définie dans string.h pour cela)
*/
void defilement(void)
{
    memmove(ptr_mem(0,0), ptr_mem(1,0), 2 * ((NB_LIG-1) * NB_LIG + (NB_COL-1)));

    // Mise à jour de la position du curseur
    position_curseur_lig --;
}

void console_putbytes(const char *s, int len)
{
    for(int i = 0; i < len; i++)
    {
        traite_car(s[i]);
    }
}