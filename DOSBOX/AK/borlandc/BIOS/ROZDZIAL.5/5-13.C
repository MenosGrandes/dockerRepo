/* 
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 5-13.  Sprawdzenie czy wyjmowano dyskietk�
*/


/*
   Sprawdzenie czy drzwiczki podanej stacji dyskietek
   by�y otwierane (w systemach, kt�re obs�uguj�
   sygna� zmiany no�nika)

   Zwraca:   0 = drzwiczki NIE BY�Y otwierane
             1 = nieprawid�owy numer stacji dyskietek
             6 = drzwiczki BY�Y otwierane
*/

unsigned char getChangeSignal( unsigned char drive )
{
union REGS regs;

regs.h.ah = 0x16;             /* AH = 16h - pobranie stanu sygna�u
                                      zmiany no�nika */
regs.h.dl = drive;            /* DL = nap�d. 0 dla A lub 1 dla B */
int86( 0x13, &regs, &regs );  /* Wywo�anie funkcji BIOS */
return ( regs.h.ah );         /* AH = 0, 1, lub 6 jak wy�ej */
}
