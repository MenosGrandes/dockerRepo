/*
   BIOS dla programujących w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 4-1.  Funkcje podająca i ustawiająca tryb pracy
                 karty graficznej
*/


/* Ustaw tryb karty graficznej */
void setVideoMode( unsigned char newMode )
{
union REGS regs;

regs.h.ah = 0;                   /* AH = 0 - ustaw tryb karty graficznej */
regs.h.al = newMode;             /* AL = tryb */
int86( 0x10, &regs, &regs );     /* Wywołanie funkcji BIOS */
}                                /* Niczego nie zwracamy */


/* Odczytaj bieżący tryb karty graficznej */
unsigned char getVideoMode( void )
{
union REGS regs;

regs.h.ah = 0x0f;               /* AH = 0x0F - odczytaj stan karty */
int86( 0x10, &regs, &regs );    /* Wywołanie funkcji BIOS */
return( regs.h.al );            /* AL = bieżący tryb */
}
