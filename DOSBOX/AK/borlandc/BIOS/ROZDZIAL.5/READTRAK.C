/*
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 5-6.  Odczytanie dyskietki �cie�ka po �cie�ce
*/


/*
   READTRAK.C
   Odczytanie ca�ej dyskietki (znanego typu) �cie�ka po �cie�ce
*/

#include <dos.h>

/* Prototyp funkcji */
unsigned char readDisketteSectors( unsigned char, unsigned char, 
                                   unsigned char, unsigned char, 
                                   unsigned char, unsigned char far * );

#define LASTHEAD         1  /* 2 g�owice (0 i 1) */
#define LASTTRACK       79  /* 80 �cie�ek (0 - 79) */
#define LASTSECTOR      15  /* 15 sektor�w na �cie�ce (1 - 15) */
#define BYTESPERSECTOR 512  /* 512 bajt�w w sektorze */
#define driveToRead      0  /* Nap�d A = 0 */

char diskBuffer[ BYTESPERSECTOR * LASTSECTOR ];

void main( void )
{
unsigned char currentHead,
              currentTrack;

for( currentTrack = 0; 
        currentTrack <= LASTTRACK; 
           currentTrack++ )
   for( currentHead = 0; 
           currentHead <= LASTHEAD; 
              currentHead++ )
/* Odczytanie 15 sektor�w poczynaj�c od sektora nr 1 */
      readDisketteSectors( driveToRead,
                           LASTSECTOR,
                           currentHead,
                           currentTrack, 
                           1,
                           diskBuffer );
}

/*
   Odczytanie sektor�w z dyskietki.
   Zwraca 0 lub kod b��du przerwania 13h.
*/
unsigned char readDisketteSectors( unsigned char drive, 
                                   unsigned char sectorsToRead, 
                                   unsigned char head, 
                                   unsigned char track,
                                   unsigned char sector, 
                                   unsigned char far *bufferPtr )
{
union REGS regs;
struct SREGS sregs;

regs.h.ah = 2;                 /* AH = 2 - czytaj sektory */
regs.h.al = sectorsToRead;     /* AL = liczba sektor�w do odczytania */
regs.x.bx = FP_OFF(bufferPtr); /* BX = przesuni�cie (offset) bufora */
regs.h.dh = head;              /* DH = g�owica */
regs.h.dl = drive;             /* DL = numer nap�du */
regs.h.ch = track;             /* CH = �cie�ka */
regs.h.cl = sector;            /* CL = numer sektora */
sregs.es  = FP_SEG(bufferPtr); /* ES = segment bufora */
int86x( 0x13, &regs, 
        &regs, &sregs );       /* Wywo�anie funkcji BIOS */
return( regs.h.ah );           /* AH = 0 lub kod b��du */
}
