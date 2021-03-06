/*
   BIOS dla programujących w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 8-7.  Wlączenie/wyłączenie działania klawisza Print Screen
*/


/* Wyłączenie klawisza Print Screen */

#define pStat (unsigned char far *)0x00500000

void disablePrintScrn( void )
{
#define INPROGRESS 1         /* 1 = trwa drukowanie */

*pStat = INPROGRESS;         /* Powoduje ignorowanie przez BIOS
                                żądania drukowania zawartości ekranu */
}


/* Włączenie klawisza Print Screen */
void enablePrintScrn( void )
{
#define COMPLETE 0           /* 0 = drukowanie zakończone */

*pStat = COMPLETE;           /* BIOS akceptuje żądanie druku */
}
