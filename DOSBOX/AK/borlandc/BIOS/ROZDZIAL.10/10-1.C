/*
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 10-1.  Odczytanie informacji o gnie�dzie EISA
*/


/*
   Odczytanie informacji o wybranym gnie�dzie EISA.
   Zwraca 0 je�eli operacja udana lub kod b��du, je�eli nie.
*/

struct EISASlotInfo
{ unsigned int  boardIdHighPart;
  unsigned int  boardIdLowPart;
  unsigned char boardInfo;
  unsigned char cfgUtilMajorVer;
  unsigned char cfgUtilMinorVer;
  unsigned int  checkSum;
  unsigned char functionCount;
  unsigned char functionSummary;
  };

unsigned int readEISASlotInfo( unsigned char slot,
                               struct EISASlotInfo far *slotInfoP )
{
union REGS regs;
struct SREGS sregs;

regs.x.ax = 0xd800;                         /* AX = D800h - podaj info */
regs.h.cl = slot;                           /* CL = numer gniazda */
int86x( 0x15, &regs, &regs, &sregs );       /* Wywo�anie funkcji BIOS */

if ( regs.x.cflag == 0 )                    /* Je�eli OK, */
{
   slotInfoP->boardIdHighPart = regs.x.si;  /* SI = ID karty starsze
                                                            16 bit�w */
   slotInfoP->boardIdLowPart = regs.x.di;   /* DI = ID karty m�odsze
                                                            16 bit�w */
   slotInfoP->boardInfo = regs.h.al;        /* AL = informacja o karcie */
   slotInfoP->cfgUtilMajorVer = regs.h.bh;  /* BH = g�. nr wersji */
   slotInfoP->cfgUtilMinorVer = regs.h.bl;  /* BL = dod. nr wersji */
   slotInfoP->checkSum = regs.x.cx;         /* CX = suma kontrolna */
   slotInfoP->functionCount = regs.h.dh;    /* DH = licznik funkcji */
   slotInfoP->functionSummary = regs.h.dl;  /* DL = podsumowanie funkcji */
   }

return( regs.h.ah );                       /* AH = 0 gdy sukces lub */
}                                          /*    = kod b��du        */
