/* 
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 9-8.  Wstrzymanie wykonywania programu
*/


/*
   Wstrzymanie wykonywania programu przez n mikrosekund.  
   Zwraca:
      0 = program zosta� wstrzymany
      1 = program NIE zosta� wstrzymany, poniewa� timer jest zaj�ty
*/

unsigned int suspendProgram( unsigned long timeValue )
{
union REGS regs;

regs.h.ah = 0x86;                /* AH = 86h - wstrzymaj prac� programu */
regs.x.cx = (timeValue >> 16);   /* CX = starsze 16 bit�w czasu */
regs.x.dx = timeValue;           /* DX = m�odsze 16 bit�w czasu */
int86( 0x15, &regs, &regs );     /* Wywo�anie funkcji BIOS */

return( regs.x.cflag );          /* Zwracamy znacznik Carry */
}
