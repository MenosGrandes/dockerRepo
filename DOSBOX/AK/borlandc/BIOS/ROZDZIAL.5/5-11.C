/* 
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 5-11.  Okre�lenie parametr�w dyskietki
*/


/* 
   Wyznaczenie parametr�w zainstalowanej dyskietki
*/


/* 
   Funkcja zawraca:
      0 = stacja nie zainstalowana lub nieznany typ no�nika (dyskietki)
      1 = dyskietka 360KB
      2 = dyskietka 1.2MB
      3 = dyskietka 720KB
      4 = dyskietka 1.44MB
*/

#include <dos.h>

#define M_UNKNOWN 0
#define M_360KB   1
#define M_12MB    2
#define M_720KB   3
#define M_144MB   4

/* Prototypy funkcji */
unsigned char readDisketteSectors( unsigned char, unsigned char, 
                                   unsigned char, unsigned char, 
                                   unsigned char, unsigned char far * );
unsigned char resetDrive( unsigned char );

/* Wyznaczenie rodzaju no�nika */
unsigned char getMediaSize( unsigned char drive )
{
#define ONE_SECTOR      1
#define HEAD_0          0
#define TRACK_0         0
#define SECTOR_1        1
#define SECTOR_9        9
#define SECTOR_18      18

#define driveA (unsigned char far *)0x00400090
#define driveB (unsigned char far *)0x00400091

unsigned char mediaStatus;
char diskBuffer[512];

/* "Pozorna" instrukcja czytania - ustawienie BDA */
readDisketteSectors( drive, ONE_SECTOR, HEAD_0, 
                      TRACK_0, SECTOR_1, diskBuffer );

/* Powt�rzenie - za pierwszym razem mog�o si� nie powie��
   z powodu sygna�u zmiany no�nika */
resetDrive( drive );
if ( readDisketteSectors(drive, ONE_SECTOR, HEAD_0, 
                           TRACK_0, SECTOR_1, diskBuffer) != 0 )
   return( M_UNKNOWN );      /* Tym razem b��d jest prawdziwy */

if ( drive == 0 )            /* Je�eli stacja A, */
   mediaStatus = *driveA;    /* czytaj z 0x00400090 */
else                         /* w przeciwnym wypadku */
   mediaStatus = *driveB;    /* stacja B pod adresem 0x00400091 */

mediaStatus &= 7;            /* Usuni�cie nadmiarowych bit�w */

/* Informacja o dyskietce 5.25" */
   switch ( mediaStatus )
   {
      case 0:   /* dyskietka 360KB w stacji 360KB - nie zweryfikowana */
      case 1:   /* dyskietka 360KB w stacji 1.2MB - nie zweryfikowana */
      case 2:   /* dyskietka 1.2MB w stacji 1.2MB - nie zweryfikowana */
         return( M_UNKNOWN );              /* Nieznany no�nik! */

      case 3:   /* dyskietka 360KB w stacji 360KB - zweryfikowana     */
      case 4:   /* dyskietka 360KB w stacji 1.2MB - zweryfikowana     */
         return( M_360KB );

      case 5:   /* dyskietka 1.2MB w stacji 1.2MB - zweryfikowana     */
         return( M_12MB );
      }

/* 
   To jest dyskietka 3.5" - sprawdzamy czy to 720KB, czy te� 1.44MB
   czytaj�c sektory 9 i 18 �cie�ki 0 i g�owicy 0.
*/
if ( readDisketteSectors(drive, ONE_SECTOR, HEAD_0, 
                           TRACK_0, SECTOR_9, diskBuffer) != 0 )
   return( M_UNKNOWN );      /* B��d - nie wiadomo co to jest */

if ( readDisketteSectors(drive, ONE_SECTOR, HEAD_0, 
                           TRACK_0, SECTOR_18, diskBuffer) == 0 )
   return( M_144MB );        /* Sektor 18 istnieje - dyskietka 1.44MB */
return( M_720KB );           /* Tylko sektor 9 - dyskietka 720KB */
}

/* Reset nap�du i sterownika */
unsigned char resetDrive( unsigned char drive )
{
union REGS regs;

regs.h.ah = 0;                /* AH = 0 - reset nap�du i sterownika */
regs.h.dl = drive;            /* DL = numer nap�du */
int86( 0x13, &regs, &regs );  /* Wywo�anie funkcji BIOS */
return( regs.h.ah );          /* AH = 0 i Carry = 0 je�eli OK */
}                             /* AH = kod b��du i Carry = 1 przy b��dzie */

/*
   Czytanie sektor�w z dyskietki.
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
