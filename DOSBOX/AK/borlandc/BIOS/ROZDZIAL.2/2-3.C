/*
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 2-3.  Tworzenie dalekich wska�nik�w przy u�yciu
                 makrodefinicji MK_FP
*/


/*
   U�ycie funkcji 8 przerwania 13h do zademonstrowania
   konwersji adresu w postaci segment:przesuni�cie (offset)
   do zmiennej za pomoc� makrodefinicji MK_FP dost�pnej
   w kompilatorach firmy Borland.
*/

unsigned char far *getDriveParameterTable( unsigned char whichDrive )
{
union REGS regs;
struct SREGS sregs;
unsigned char far *tablePtr;

/* Pocz�tkowo wska�nik = NULL na wypadek niepowodzenia funkcji 8 */
tablePtr = NULL;

regs.h.ah = 8;                          /* AH = funkcja 8 */
regs.h.dl = whichDrive;                 /* DL = numer nap�du */
int86x( 0x13, &regs, &regs, &sregs );   /* Wywo�anie funkcji BIOS */

/* 
   Je�eli operacja powiod�a si�, to sregs.es zawiera
   numer segmentu, a regs.x.di zawiera przesuni�cie (offset).
   MK_FP tworzy na ich podstawie daleki wska�nik. Pierwszym
   argumentem jest numer segmentu, drugim przesuni�cie.
*/

if ( regs.x.cflag == 0 )                /* Je�eli OK, */
   tablePtr = MK_FP( sregs.es, regs.x.di );

/* Zwracamy wska�nik do funkcji wywo�uj�cej (lub NULL
   je�eli funkcja przerwania 13h nie zosta�a poprawnie
   wykonana).
*/
return( tablePtr );
}
