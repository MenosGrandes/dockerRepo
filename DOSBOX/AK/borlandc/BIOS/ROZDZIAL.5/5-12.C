/*
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 5-12.  Okre�lenie czy obs�ugiwany jest sygna� zmiany dyskietki
*/


/*
   Sprawdzenie obs�ugi sygna�u zmiany no�nika
*/


/*
   Informuje czy wybrany nap�d jest dost�pny
   oraz, czy obs�uguje on sygna� zmiany no�nika

   Zwraca:   0 = stacja NIE zainstalowana
             1 = stacja zainstalowana BEZ obs�ugi sygna�u zmiany no�nika
             2 = stacja zainstalowana Z obs�ug� sygna�u zmiany no�nika
             3 = to jest twardy dysk
*/

unsigned char testDriveInstalled( unsigned char drive )
{
union REGS regs;

regs.h.ah = 0x15;             /* AH = 15h - sprawdzenie
                                      czy nap�d jest zainstalowany */
regs.h.dl = drive;            /* DL = numer nap�du */
int86( 0x13, &regs, &regs );  /* Wywo�anie funkcji BIOS */
return ( regs.h.ah );         /* AH = 0, 1, 2, 3 jak wy�ej */
}
