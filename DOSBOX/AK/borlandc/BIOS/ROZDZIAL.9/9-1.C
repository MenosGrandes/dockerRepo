/* 
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 9-1.  Odczytanie licznika systemowego
*/


/*
   Zapisanie do zmiennej programu wywo�uj�cego stanu licznika
   systemowego i zwr�cenie stanu znacznika przekroczenia p��nocy
*/

unsigned char readTickCounter( unsigned long *callvar )
{
union REGS regs;
unsigned long result;

regs.h.ah = 0;                /* AH = 0 - odczytanie licznika */
int86( 0x1a, &regs, &regs );  /* Wywo�anie funkcji BIOS */

result = regs.x.cx;           /* CX = starsze 16 bit�w */
result <<= 16;                /* przesuni�cie ich na w�a�ciw� pozycj� */
result |= regs.x.dx;          /* DX = m�odsze 16 bit�w */
*callvar = result;            /* Zapisanie do zmiennej
                                      programu wywo�uj�cego */

return( regs.h.al );          /* AL = znacznik przekroczenia p��nocy */
}
