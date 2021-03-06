/* ***************************************************
   LPTSTAT.C
   Copyright (c) 1992 - Billy P. Taylor
   ***************************************************
   Program wy�wietlaj�cy informacj� o stanie drukarki
   w r��nych warunkach, np. wy��czona drukarka, brak
   papieru itp. 
   Uwaga dla u�ytkownik�w sieci:
   Prosz� pami�ta�, �e oprogramowanie sieci mo�e
   zast�powa� standardow� obs�ug� drukarki w�asnymi
   sterownikami. W takich przypadkach testowanie stanu
   wsp�lnej drukarki sieciowej mo�e wskazywa� stan
   niezgodny z rzeczywisto�ci�.

   Uruchomienie: LPTSTAT<Enter>.
   ************************************************** */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define boolean unsigned char
#define YES 1
#define NO  0

#define BDA_lpt_addr (unsigned int  far *)0x00400008
#define BDA_lpt_to   (unsigned char far *)0x00400078

#define LPT_ADDRESS(which) *(BDA_lpt_addr+which)
#define LPT_TIMEOUT(which) *(BDA_lpt_to+which)

#define FORMFEED 12

/* ***********  Zmienne  *********** */
union REGS regs;

boolean sendDataToPrinter = NO,
        somethingPrinted = NO,
        quitNow   = NO;

unsigned int currentPrinter = 0;

/* *********  Komunikaty  ******** */

char *printerNames[] = { "LPT1","LPT2","LPT3" };

char title[]       = 
"                 LPTSTAT : Wy�wietlanie s�owa stanu portu drukarki",
     dashes[]      = 
"--------------------------------------------------------------------------------",
     blankLine[]   = 
"                                                                                ",
     prtDisabled[] = "Drukowanie wy��czone",
     prtEnabled[]  = "Drukowanie w��czone ";

char *screen[] =
{
title,
dashes,
"Program ten wy�wietla w czasie rzeczywistym s�owo stanu wybranego portu drukarki",
"Wy��czaj drukark�, wyjmuj z niej papier, prze��czaj ON/OFF LINE itp.",
"obserwuj�c jednocze�nie zmiany warto�ci s�owa stanu dla r��nych sytuacji.",
blankLine,
"Bit Stan    Funkcja ",
" 7  � . - Drukarka gotowa",
" 6  � . - Potwierdzenie",
" 5  � . - Brak papieru",
" 4  � . - Drukarka wybrana",
" 3  � . - B��d wej�cia/wyj�cia",
" 2  � . - Zarezerwowany",
" 1  � . - Zarezerwowany",
" 0  � . - Czas min��",
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
"Naci�nij: 1 = LPT1    D = Drukuj",
"          2 = LPT2    P = Przerwij drukowanie",
"          3 = LPT3    K = Koniec",
0 };

/* 
   Okre�lenie wierszy i kolumn, w kt�rych wy�wietlane
   b�d� informacje o stanie drukarki.
   Zak�adany jest ekran 80x25 znak�w. Tryb taki
   ustawiany jest w funkcji main() (tryb 3).
*/
#define TEXT_MODE           3  /* Tryb pracy ekranu */

#define PADDROW    18     /* Wiersz z informacj� o adresie portu */
#define PLEGENDROW 20     /* Wiersz z informacj� o zainstalowaniu portu */
#define PSTATROW    7     /* Wiersz ze stanem drukarki */
#define PSTATCOL    6     /* Kolumna ze stanem drukarki */

char stringToPrint[]  = 
"\015AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789\n";

char *nextByteToPrint = stringToPrint;

/* *******  Prototypy funkcji  ******** */
unsigned int  getPrinterStatus    ( unsigned int );
unsigned char getVideoMode        ( void );
unsigned int  printChar           ( unsigned int, 
                                    unsigned char );
void          say                 ( unsigned int, 
                                    unsigned int, 
                                    char * );
void          sayPrinterName      ( unsigned int );
void          sendByteToPrinter   ( void );
void          serviceKeyboard     ( void );
void          setCursorPosition   ( unsigned int, 
                                    unsigned int );
void          setVideoMode        ( unsigned char );
void          showPrinterStatus   ( unsigned int, 
                                    unsigned int, 
                                    unsigned int );
void          writeChar           ( unsigned int, 
                                    unsigned int, 
                                    unsigned char );

/* ***********  Main  *********** */
void main( void )
{
unsigned int row;

/* Ustawienie trybu tekstowego 80x25 i przygotowanie ekranu */
setVideoMode( TEXT_MODE );
for( row=0; screen[row] != 0; row++ )
   say( row, 0, screen[row] );

/* Informacja, kt�ra drukarka jest wybrana */
sayPrinterName( currentPrinter );
say( PLEGENDROW, 0, prtDisabled );

/* P�tla g��wna */
while ( !quitNow )
{
/* Analizuj dane z klawiatury */
   serviceKeyboard();

/* Je�eli trzeba, wydrukuj nast�pn� lini� tekstu */
   if ( sendDataToPrinter )
      sendByteToPrinter();

/* Wy�wietl stan drukarki */
   showPrinterStatus( currentPrinter, PSTATROW, PSTATCOL );
   }

/* Wysu� ostatni� kartk� papieru */
if ( somethingPrinted )
   printChar( currentPrinter, FORMFEED );

/* Powr�t do DOSu */
setCursorPosition( 24, 0 );
exit( 0 );
}

/* Obs�uga polece� wydawanych z klawiatury */
void serviceKeyboard( void )
{
if ( kbhit() )
{
   switch ( getch() )
   {
      case '1' : currentPrinter = 0;
                 sayPrinterName( currentPrinter );
                 break;
      case '2' : currentPrinter = 1;
                 sayPrinterName( currentPrinter );
                 break;
      case '3' : currentPrinter = 2;
                 sayPrinterName( currentPrinter );
                 break;
      case 'p' :
      case 'P' : if ( sendDataToPrinter )
                 {
                    say( PLEGENDROW, 0, prtDisabled );
                    sendDataToPrinter = NO;
                    }
                 break;
      case 'k' :
      case 'K' : quitNow = YES;
                 break;
      case 'd' :
      case 'D' : if ( LPT_ADDRESS(currentPrinter) != 0 )
                 {
                    say( PLEGENDROW, 0, prtEnabled );
                    sendDataToPrinter = YES;
                    }
                 break;
      }
   }
}

/* Wydrukowanie nast�pnego znaku z tablicy stringToPrint[]
   i przesuni�cie wska�nika na nast�pny znak.
   Na ko�cu tablicy, przestawiamy wska�nik na pocz�tek.
*/
void sendByteToPrinter( void )
{
printChar( currentPrinter, *nextByteToPrint );
somethingPrinted = YES;

++nextByteToPrint;
if ( *nextByteToPrint == 0 )
   nextByteToPrint = stringToPrint;
}

/* Drukowanie znaku */
unsigned int printChar( unsigned int whichPrinter, 
                        unsigned char character )
{
regs.h.ah = 0;                     /* AH = 0 - drukuj znak */
regs.h.al = character;             /* AL = znak */
regs.x.dx = whichPrinter;          /* DX = numer drukarki */
int86( 0x17, &regs, &regs );       /* Wywo�anie funkcji BIOS */
return( regs.h.ah );               /* AH = stan */
}

/* Pobranie informacji o stanie portu drukarki */
unsigned int getPrinterStatus( unsigned int whichPrinter )
{
regs.h.ah = 2;                     /* AH = 2 - pobierz s�owo stanu drukarki */
regs.x.dx = whichPrinter;          /* DX = numer drukarki */
int86( 0x17, &regs, &regs );       /* Wywo�anie funkcji BIOS */
return( regs.h.ah );               /* AH = stan */
}

/* Ustawienie trybu karty graficznej */
void setVideoMode( unsigned char newMode )
{
union REGS regs;

regs.h.ah = 0;                     /* AH = 0 - ustaw tryb karty graficznej */
regs.h.al = newMode;               /* AL = tryb */
int86( 0x10, &regs, &regs );       /* Wywo�anie funkcji BIOS */
}

/* Pobranie bie��cego trybu karty graficznej */
unsigned char getVideoMode( void )
{
union REGS regs;

regs.h.ah = 0x0f;                  /* AH = 0Fh - pobierz tryb karty graficznej */
int86( 0x10, &regs, &regs );       /* Wywo�anie funkcji BIOS */
return( regs.h.al );               /* AL = bie��cy tryb */
}

/* Przemieszczenie kursora do wybranego wiersza i kolumny */
void setCursorPosition( unsigned int row,
                        unsigned int column )
{
regs.h.ah = 2;                     /* AH = 2 - Przesu� kursor */
regs.h.bh = 0;                     /* BH = strona */
regs.h.dh = row;                   /* DH = wiersz */
regs.h.dl = column;                /* DL = kolumna */
int86( 0x10, &regs, &regs );       /* Wywo�anie funkcji BIOS */
}

/* Wy�wietlenie znaku we wskazanym wierszu i kolumnie */
void writeChar( unsigned int row, 
                unsigned int column, 
                unsigned char character)
{
setCursorPosition( row, column );

regs.h.ah = 0x0a;                  /* AH = 0Ah - napisz znak */
regs.h.al = character;             /* AL = znak */
regs.h.bh = 0;                     /* BH = strona */
regs.x.cx = 1;                     /* CX = krotno�� znaku */

int86( 0x10, &regs, &regs );       /* Wywo�anie funkcji BIOS */
}

/* Wy�wietlenie stanu wybranej drukarki */
void showPrinterStatus( unsigned int whichPrinter, 
                        unsigned int row, 
                        unsigned int col )
{
unsigned char status;

status = getPrinterStatus( whichPrinter );

writeChar( row++, col, (char)('0' + ((status & 128) != 0)) );
writeChar( row++, col, (char)('0' + ((status &  64) != 0)) );
writeChar( row++, col, (char)('0' + ((status &  32) != 0)) );
writeChar( row++, col, (char)('0' + ((status &  16) != 0)) );
writeChar( row++, col, (char)('0' + ((status &   8) != 0)) );
writeChar( row++, col, (char)('0' + ((status &   4) != 0)) );
writeChar( row++, col, (char)('0' + ((status &   2) != 0)) );
writeChar( row,   col, (char)('0' + ((status &   1) != 0)) );
}

/* Wy�wietlenie nazwy i adresu portu */
void sayPrinterName( unsigned int whichPrinter )
{
say( PADDROW, 0, blankLine );

setCursorPosition( PADDROW, 0 );
printf( "%s ", printerNames[whichPrinter] );

if ( LPT_ADDRESS(whichPrinter) == 0 )
{
   printf( "nie jest zainstalowany." );
   say( PLEGENDROW, 0, prtDisabled );
   sendDataToPrinter = NO;
   }
else
   printf( "jest zainstalowany w porcie %.4X - maks. czas oczekiwania %u sek.",
            LPT_ADDRESS(whichPrinter),
            LPT_TIMEOUT(whichPrinter) );
}

/* Wy�wietlenie tekstu w wybranym wierszu i kolumnie */
void say( unsigned int row, 
          unsigned int column, 
          char *stringPtr )
{
setCursorPosition( row, column );
printf( "%s", stringPtr );
}

