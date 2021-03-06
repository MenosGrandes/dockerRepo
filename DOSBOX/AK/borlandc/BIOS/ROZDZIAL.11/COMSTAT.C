/* *********************************************************
   COMSTAT.C 
   Copyright (c) 1992 - Billy P. Taylor
   *********************************************************
   Pokazuje spos�b u�ycie funkcji przerwania 14h do
   konfiguracji port�w szeregowych, odczytywania stanu
   linii i modemu, wysy�ania i odbierania danych.

   UWAGA: program ten wymaga kompilatora potrafi�cego kompilowa�
          funkcje _interrupt, takiego jak Microsoft C wersja 5.1
          lub nowsze oraz Borland C++ 2.0 lub nowszy.

   Aby uruchomi� program napisz COMSTAT<Enter>.
   ******************************************************* */

/* ******** STA�E i MAKRODEFINICJE ********* */
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#define BIT0   1  /* 00000001 */
#define BIT1   2  /* 00000010 */
#define BIT2   4  /* 00000100 */
#define BIT3   8  /* 00001000 */
#define BIT4  16  /* 00010000 */
#define BIT5  32  /* 00100000 */
#define BIT6  64  /* 01000000 */
#define BIT7 128  /* 10000000 */

/* Bity stanu linii */
#define Timeout            BIT7
#define TxShiftRegEmpty    BIT6
#define TxHoldingRegEmpty  BIT5
#define BreakDetected      BIT4
#define FramingError       BIT3
#define ParityError        BIT2
#define ReceiverOverrun    BIT1
#define DataReceived       BIT0

/* Bity stanu modemu */
#define CarrierDetect      BIT7
#define RingIndicate       BIT6
#define DataSetReady       BIT5
#define ClearToSend        BIT4
#define DeltaCD            BIT3
#define TrailingEdgeRI     BIT2
#define DeltaDSR           BIT1
#define DeltaCTS           BIT0

/* Bity funkcji 00h przerwania 14h */
#define BAUD110               0                  /* 00000000 */
#define BAUD150            BIT5                  /* 00100000 */
#define BAUD300            BIT6                  /* 01000000 */
#define BAUD600          ( BIT6 + BIT5 )         /* 01100000 */
#define BAUD1200           BIT7                  /* 10000000 */
#define BAUD2400         ( BIT7 + BIT5 )         /* 10100000 */
#define BAUD4800         ( BIT7 + BIT6 )         /* 11000000 */
#define BAUD9600         ( BIT7 + BIT6 + BIT5 )  /* 11100000 */
#define CLEAR_BAUD_RATE  ( BIT4 + BIT3 + BIT2 + BIT1 + BIT0 )

#define NOPARITY              0                  /* 00000000 */
#define ODDPARITY          BIT3                  /* 00001000 */
#define EVENPARITY       ( BIT4 + BIT3 )         /* 00011000 */
#define CLEAR_PARITY     ( BIT7 + BIT6 + BIT5 + BIT2 + BIT1 + BIT0 )

#define STOP1                 0                  /* 00000000 */
#define STOP2              BIT2                  /* 00000100 */
#define CLEAR_STOP_BITS  ( BIT7 + BIT6 + BIT5 + BIT4 + BIT3 + BIT1 + BIT0 )

#define DATA7              BIT1                  /* 00000010 */
#define DATA8            ( BIT1 + BIT0 )         /* 00000011 */
#define CLEAR_DATA_BITS  ( BIT7 + BIT6 + BIT5 + BIT4 + BIT3 + BIT2 )

#define BACKSPACE  8
#define ENTER     13
#define ESCAPE    27
#define NEWLINE   10

#define boolean unsigned char
#define YES 1
#define NO  0

#define BDA_com_addr (unsigned int  far *)0x00400000
#define BDA_com_to   (unsigned char far *)0x0040007c

#define COM_ADDRESS(which) *(BDA_com_addr+which)
#define COM_TIMEOUT(which) *(BDA_com_to+which)

/* Makrodefinicja zwracaj�ca '1' je�eli bit = 1 lub '0' je�eli bit = 0 */
#define bitOnOff( value, bit ) (value & bit) ? '1' : '0'

/* ********  Zmienne globalne  ******** */

union REGS regs;

boolean quitNow = NO,
        converseModeOn = NO;

unsigned char  scratch[100],
               kbdBuffer[81] = "\000",
              *kbdBufferP = kbdBuffer;

unsigned int currentPort = 0,
             lineCol     = 1,
             cursorCol   = 1;

unsigned char cfgWords[4] = 
   { BAUD1200+NOPARITY+STOP1+DATA8,     /* COM1 */
     BAUD1200+NOPARITY+STOP1+DATA8,     /* COM2 */
     BAUD1200+NOPARITY+STOP1+DATA8,     /* COM3 */
     BAUD1200+NOPARITY+STOP1+DATA8 };   /* COM4 */

/* ********  Dane do interpretacji (rozdzielania) polece�  ******** */

struct dispatchEntry
{
   char *command;
   void far *function;
   };

void changeComPorts     ( void );
void sayBadCommand      ( void );
void sendString         ( void );
void setExitFlag        ( void );
void setParity          ( void );
void setSpeed           ( void );
void setStopBits        ( void );
void setTimeout         ( void );
void setWordSize        ( void );
void showHelp           ( void );
void turnConverseModeOn ( void );

struct dispatchEntry dispatchTable[] = 
{
   { "KONIEC",      setExitFlag },
   { "POMOC",      showHelp },
   { "COM",       changeComPorts },
   { "PARZYSTOSC",    setParity },
   { "BITYSTOPU",  setStopBits },
   { "BITYDANYCH",  setWordSize },
   { "SZYBKOSC",     setSpeed },
   { "CZAS",   setTimeout },
   { "NADAJ",      sendString },
   { "ROZMOWA",  turnConverseModeOn },
   { 000,         sayBadCommand }
   };

/* *************************  Komunikaty  ************************* */

char *comPortNames[] = { "COM1","COM2","COM3","COM4" };

char blankLine[] = 
"                                                                                ",
     dashes[]    = 
"--------------------------------------------------------------------------------";

char *mainScreen[] =
{
"-------------- COMSTAT : Program do testowania portu szeregowego ---------------",
blankLine,
"                       --- Stan linii --------- Bit ------- Stan modemu --------",
"Port      :             Przekroczony czas - . -- 7 -- . - No�na odbioru (CD)",
"Adres     :             Pusty rej. przes. - . -- 6 -- . - Dzwonek (RI)",
"Czas      :             Pusty rej. danych - . -- 5 -- . - DSR",
"Szybko��  :            Przerwa w odbiorze - . -- 4 -- . - CTS",
"Parzysto��:                    B��d ramki - . -- 3 -- . - DCD",
"Dane      :              B��d parzysto�ci - . -- 2 -- . - Koniec dzwonka",
"Bity stopu:                 Utracono dane - . -- 1 -- . - DDSR",
"           Odebrane dane w s� w rejestrze - . -- 0 -- . - DCTS",
dashes,
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
blankLine,
dashes,
blankLine,
blankLine,
blankLine,
"Napisz polecenie lub POMOC.",
":",
0 };

/* 
   Okre�lenie wierszy i kolumn ekranu do wy�wietlania stanu.
   Zak�adane jest u�ycie ekranu 80x25 w trybie tekstowym.
   Ustawiamy taki poprzez wybranie trybu graficznego 3 w funkcji main().
*/
#define TEXT_MODE          3     /* Tryb graficzny */
#define PORTNAMEROW       12     /* Nazwa portu */
#define PSTATROW           3     /* Wiersz, w kt�rym zaczyna
                                    si� wy�wietlanie stanu */
#define LINESTATCOL       44     /* Kolumna stanu linii */
#define MODEMSTATCOL      54     /* Kolumna stanu modemu */
#define LINE_WINTOPROW    12     /* G�rny wiersz okna odbieranych danych */
#define LINE_WINLEFTCOL    0     /* Lewa kolumna okna odbieranych danych */
#define LINE_WINBOTTOMROW 18     /* Dolny wiersz okna odbieranych danych */
#define LINE_WINRIGHTCOL  79     /* Prawa kol. okna odbieranych danych */

#define CMD_WINTOPROW     20     /* G�rny wiersz okna polece� */
#define CMD_WINLEFTCOL     0     /* Lewa kolumna okna polece� */
#define CMD_WINBOTTOMROW  23     /* Dolny wiersz okna polece� */
#define CMD_WINRIGHTCOL   79     /* Prawa kolumna okna polece� */

#define STATROW            3     /* Wiersz stanu portu */
#define STATCOL           12     /* Kolumna stanu portu */

#define CURSORROW         24     /* Wiersz wprowadzania polece� */

char *helpLines[] =
{
"Koniec, Com <1,2,3,4>, Parzystosc <norm,niep,brak>, BityStopu <1,2>",
"BityDanych <5,6,7,8>, Szybkosc <110,150,300,600,1200,2400,4800,9600>",
"Czas <1..255>, Nadaj <tekst>, Rozmowa",
0 };

/* *******************  Prototypy funkcji  ******************** */

void          checkForRcvdData       ( void );
void          clearScreen            ( void );
void          dispatchCommand        ( void );
unsigned int  getComPortStatus       ( void );
unsigned char getVideoMode           ( void );
void          say                    ( unsigned int, 
                                       unsigned int, 
                                       char * );
void          scrollCmdWindow        ( void );
void          scrollLineWindow       ( void );
void          serviceKeyboard        ( void );
void          setComParameters       ( void );
void          setCursorPosition      ( unsigned int,
                                       unsigned int );
void          setVideoMode           ( unsigned char );
void          showPortStatus         ( unsigned int, 
                                       unsigned int, 
                                       unsigned int );
void          showSettings           ( void );
unsigned int  transmitChar           ( unsigned char );
void          writeChar              ( unsigned int, 
                                       unsigned int, 
                                       unsigned char );
void          writeCharToLineWindow  ( char );
void          writeWindowString      ( char * );


/* ********  Main  ******* */
void main( void )
{
unsigned int lineNumber;

/* Narysuj ekran roboczy */
setVideoMode( TEXT_MODE );
for( lineNumber=0; mainScreen[lineNumber] != 0; lineNumber++ )
   say( lineNumber, 0, mainScreen[lineNumber] );

/* Poka� konfiguracj� wybranego portu */
setComParameters();
showSettings();

/* G��wna p�tla */
while ( !quitNow )
{
   serviceKeyboard();
   checkForRcvdData();
   showPortStatus( PSTATROW, LINESTATCOL, MODEMSTATCOL );
   }

/* Powr�t do DOSu */
clearScreen();
printf( "\n" );
exit( 0 );
}


/* ******************  Funkcje wykonuj�ce polecenia  ****************** */

/* Polecenie KONIEC : Ustawienie znacznika m�wi�cego funkcji main(),
   �e nale�y zako�czy� prac�. */
void setExitFlag( void )
{
quitNow = YES;
   }

/* Polecenie POMOC : Wy�wietlenie spisu polece� */
void showHelp( void )
{
unsigned int lineNumber;

for( lineNumber=0; helpLines[lineNumber]!=0; lineNumber++ )
   writeWindowString( helpLines[lineNumber] );
}

/* Polecenie COM : Wybranie portu szeregowego */
void changeComPorts( void )
{
boolean portOk;

portOk = NO;

if ( strchr(kbdBuffer,'1') )
{
   currentPort = 0;
   portOk = YES;
   }

if ( strchr(kbdBuffer,'2') )
{
   currentPort = 1;
   portOk = YES;
   }

if ( strchr(kbdBuffer,'3') )
{
   currentPort = 2;
   portOk = YES;
   }

if ( strchr(kbdBuffer,'4') )
{
   currentPort = 3;
   portOk = YES;
   }

if ( portOk )
{
   setComParameters();
   showSettings();
   }
else
   sayBadCommand();
}

/* Polecenie PARZYSTOSC : Zmiana kontroli parzysto�ci dla
      u�ywanego portu */
void setParity( void )
{
boolean even,odd,none;

strupr( kbdBuffer );

even = ( strstr(kbdBuffer,"NORM") != 0 );
odd  = ( strstr(kbdBuffer,"NIEP") != 0 );
none = ( strstr(kbdBuffer,"BRAK") != 0 );

if ( (even+odd+none) != 1 )
   writeWindowString( "Mo�esz poda� jedynie NORM, NIEP lub BRAK !" );
else
{
   cfgWords[currentPort] &= CLEAR_PARITY;
   if ( even )
      cfgWords[currentPort] |= EVENPARITY;
   if ( odd )
      cfgWords[currentPort] |= ODDPARITY;
   if ( none )
      cfgWords[currentPort] |= NOPARITY;
   setComParameters();
   showSettings();
   }
}

/* Zwr�cenie adresu do nast�pnego s�owa linii polecenia */
char *findToken( char *stringPtr )
{
while ( (*stringPtr != 0) && (*stringPtr != ' ') )
   stringPtr++;

if ( *stringPtr != 0 )
{
   while ( *stringPtr == ' ' )
      stringPtr++;
   }

return( stringPtr );
}

/* Polecenie BITYSTOPU : Ustawienie liczby bit�w stopu
      dla bie��cego portu */
void setStopBits( void )
{
unsigned int stopBits;

stopBits = atoi( findToken(kbdBuffer) );

if ( (stopBits != 1) && (stopBits != 2) )
   writeWindowString( "Mo�esz poda� jedynie 1 lub 2 bity stopu!" );
else
{
   cfgWords[currentPort] &= CLEAR_STOP_BITS;
   if ( stopBits == 1 )
      cfgWords[currentPort] |= STOP1;
   else
      cfgWords[currentPort] |= STOP2;
   setComParameters();
   showSettings();
   }
}

/* Polecenie BITYDANYCH : Ustawienie liczby bit�w danych
       dla bie��cego portu */
void setWordSize( void )
{
unsigned int wordSize;

wordSize = atoi( findToken(kbdBuffer) );

if ( (wordSize != 7) && (wordSize != 8) )
   writeWindowString( "Mo�esz wybra� jedynie 7 lub 8 bit�w danych !" );
else
{
   cfgWords[currentPort] &= CLEAR_DATA_BITS;
   if ( wordSize == 7 )
      cfgWords[currentPort] |= DATA7;
   else
      cfgWords[currentPort] |= DATA8;
   setComParameters();
   showSettings();
   }
}

/* Polecenie SZYBKOSC : Ustawienie szybko�ci linii
        dla bie��cego portu */
void setSpeed( void )
{
unsigned int index, speed;

static unsigned int bps[] = 
   { 110,150,300,600,1200,2400,4800,9600,0 };

static unsigned int bity[] = 
   { BAUD110,BAUD150,BAUD300,BAUD600,
     BAUD1200,BAUD2400,BAUD4800,BAUD9600 };

speed = atoi( findToken(kbdBuffer) );

index = 0;
while( (bps[index] != speed) && (bps[index]!=0) )
   index++;

if ( bps[index] == 0 )
   writeWindowString( "SZYBKOSC 110,150,300,600,1200,2400,4800,9600 !" );
else
{
   cfgWords[currentPort] &= CLEAR_BAUD_RATE;
   cfgWords[currentPort] |= bity[index];
   setComParameters();
   showSettings();
   }
}

/* Polecenie CZAS : ustawienie dopuszczalnego czasu oczekiwania
         dla bie��cego portu */
void setTimeout( void )
{
unsigned char timeout;

timeout = atoi( findToken(kbdBuffer) );
COM_TIMEOUT( currentPort ) = timeout;
showSettings();
}

/* Polecenie NADAJ : Wys�anie tekstu przez bie��cy port szeregowy */
void sendString( void )
{
char *stringPtr;

stringPtr = findToken( kbdBuffer );

while ( *stringPtr != NULL )
{
   transmitChar( *stringPtr );
   ++stringPtr;
   }

transmitChar( ENTER );
}

/* Polecenie ROZMOWA : Uruchomienie trybu rozmowy */
void turnConverseModeOn( void )
{
converseModeOn = YES;
writeWindowString( "Naci�nij ESC, aby przerwa� rozmow�" );
}

/* DOMY�LNIE : poinformuj, �e linia polecenia jest z�a */
void sayBadCommand( void )
{
writeWindowString( "Napisz POMOC, aby wy�wietli� spis polece�." );
}

/* ********  Rozdzielanie polece�  ******** */
/* Wywo�anie funkcji odpowiadaj�cej poleceniu
   wpisanemu przez u�ytkownika */
void dispatchCommand( void )
{
void (_interrupt _far *dispatchFunction)( void );

struct dispatchEntry *dispatchTablePtr;

dispatchTablePtr = dispatchTable;

while ( (dispatchTablePtr->command != 0) &&
        (memicmp( kbdBuffer,
                  dispatchTablePtr->command,
                  strlen(dispatchTablePtr->command)) != 0) )
{
   ++dispatchTablePtr;
   }

(unsigned long)dispatchFunction = 
   (unsigned long)dispatchTablePtr->function;

(*dispatchFunction)();
}

/* ********  Funkcje pomocnicze  ******** */
/* Przewini�cie okna odbieranych danych */
void scrollLineWindow( void )
{
#define WHITE_ON_BLACK 7
#define PAGE_0         0
#define SCROLL_ONCE    1

regs.h.ah = 6;                       /* AH = 6 - przewi� ekran */
regs.h.al = SCROLL_ONCE;             /* AL = ile przewin�� */
regs.h.bh = WHITE_ON_BLACK;          /* BH = atrybut */
regs.h.bl = PAGE_0;                  /* BL = strona */
regs.h.ch = LINE_WINTOPROW;          /* CH = g�rny wiersz */
regs.h.cl = LINE_WINLEFTCOL;         /* CL = lewa kolumna */
regs.h.dh = LINE_WINBOTTOMROW;       /* DH = dolny wiersz */
regs.h.dl = LINE_WINRIGHTCOL;        /* DL = prawa kolumna */
int86( 0x10, &regs, &regs );         /* Wywo�anie funkcji BIOS */
}

/* Przewini�cie okna polece� */
void scrollCmdWindow( void )
{
#define WHITE_ON_BLACK 7
#define PAGE_0         0
#define SCROLL_ONCE    1

regs.h.ah = 6;                       /* AH = 6 - przewi� ekran */
regs.h.al = SCROLL_ONCE;             /* AL = ile przewin�� */
regs.h.bh = WHITE_ON_BLACK;          /* BH = atrybut */
regs.h.bl = PAGE_0;                  /* BL = strona */
regs.h.ch = CMD_WINTOPROW;           /* CH = g�rny wiersz */
regs.h.cl = CMD_WINLEFTCOL;          /* CL = lewa kolumna */
regs.h.dh = CMD_WINBOTTOMROW;        /* DH = dolny wiersz */
regs.h.dl = CMD_WINRIGHTCOL;         /* DL = prawa kolumna */
int86( 0x10, &regs, &regs );         /* Wywo�anie funkcji BIOS */
}

/* Napisanie tekstu w oknie polece� */
void writeWindowString( char *stringPtr )
{
scrollCmdWindow();
say( CMD_WINBOTTOMROW, CMD_WINLEFTCOL, stringPtr );
}

/* Napisanie znaku w oknie odbieranych danych */
void writeCharToLineWindow( char character )
{
switch ( character )
{
   case ENTER   : lineCol = LINE_WINLEFTCOL;
                  break;
   case NEWLINE : scrollLineWindow();
                  lineCol = LINE_WINLEFTCOL;
                  break;
   default      : writeChar( LINE_WINBOTTOMROW, lineCol, character );
                  ++lineCol;
                  if ( lineCol > LINE_WINRIGHTCOL )
                  {
                     scrollLineWindow();
                     lineCol = LINE_WINLEFTCOL;
                     }
                  break;
   }
}

/* Przesuni�cie kursora do wybranego wiersza i kolumny */
void setCursorPosition( unsigned int row, unsigned int column )
{
regs.h.ah = 2;                       /* AH = 2 - przesu� kursor */
regs.h.bh = 0;                       /* BH = strona */
regs.h.dh = row;                     /* DH = wiersz */
regs.h.dl = column;                  /* DL = kolumna */
int86( 0x10, &regs, &regs );         /* wywo�anie funkcji BIOS */
}

/* Wy�wietlenie znaku we wskazanym wierszu i kolumnie */
void writeChar( unsigned int row, 
                unsigned int column, 
                unsigned char character)
{
setCursorPosition( row, column );

regs.h.ah = 0x0a;                    /* AH = 0Ah - wy�wietl znak */
regs.h.al = character;               /* AL = znak */
regs.h.bh = 0;                       /* BH = strona */
regs.x.cx = 1;                       /* CX = liczba powt�rze� */
int86( 0x10, &regs, &regs );         /* Wywo�anie funkcji BIOS */
}

/* Wy�wietlenie tekstu we wskazanym wierszu i kolumnie */
void say( unsigned int row, 
          unsigned int column, 
          char *stringPtr )
{
while( *stringPtr != 0 )
{
   writeChar( row, column, *stringPtr );
   ++column;
   ++stringPtr;
   }
}

/* Wyczyszczenie ekranu */
void clearScreen( void )
{
setVideoMode( getVideoMode() );
}

/* Ustawienie trybu karty graficznej */
void setVideoMode( unsigned char newMode )
{
union REGS regs;

regs.h.ah = 0;                /* AH = 0 - ustaw tryb karty graficznej */
regs.h.al = newMode;          /* AL = tryb */
int86( 0x10, &regs, &regs );  /* Wywo�anie funkcji BIOS */
}

/* Odczytanie bie��cego trybu pracy karty graficznej */
unsigned char getVideoMode( void )
{
union REGS regs;

regs.h.ah = 0x0f;             /* AH = 0Fh - podaj tryb graficzny */
int86( 0x10, &regs, &regs );  /* Wywo�anie funkcji BIOS */
return( regs.h.al );          /* AL = bie��cy tryb graficzny */
}

/* ********************  Obs�uga klawiatury  ********************** */
/* 
   Odczytanie danych z klawiatury, z�o�enie ich w lini� polecenia
   i wywo�anie odpowiedniego polecenia*/
void serviceKeyboard( void )
{
unsigned char byteFromKbd;

setCursorPosition( CURSORROW, cursorCol );

if ( kbhit() )
{
   byteFromKbd = getch();
   if ( converseModeOn )
   {
      if ( byteFromKbd == ESCAPE )
      {
         converseModeOn = NO;
         writeWindowString( "ROZMOWA przerwana" );
         }
      else
         transmitChar( byteFromKbd );
      }
   else
   {
      switch ( byteFromKbd )
      {
         case ENTER     : if ( strlen(kbdBuffer) != 0 )
                          {
                             writeWindowString( kbdBuffer );
                             while( cursorCol != 0 )
                             {
                                say( CURSORROW, cursorCol, " " );
                                --cursorCol;
                                }
                             ++cursorCol;
                             dispatchCommand();
                             kbdBufferP = kbdBuffer;
                             *kbdBufferP = 0;
                             }
                          break;
         case ESCAPE    : while( cursorCol != 0 )
                          {
                             say( CURSORROW, cursorCol, " " );
                             --cursorCol;
                             }
                          ++cursorCol;
                          kbdBufferP = kbdBuffer;
                          *kbdBufferP = 0;
                          break;
         case BACKSPACE : if ( cursorCol != 1 )
                          {
                             --kbdBufferP;
                             *kbdBufferP = 0;
                             --cursorCol;
                             say( CURSORROW, cursorCol, " " );
                             }
                          break;
         default        : if ( cursorCol != (CMD_WINRIGHTCOL-1) )
                          {
                             *kbdBufferP = byteFromKbd;
                             ++kbdBufferP;
                             *kbdBufferP = 0;
                             ++cursorCol;
                             say( CURSORROW, 1, kbdBuffer );
                             }
                          break;
         }
      }
   }
}

/* ********  Funkcje obs�ugi portu szeregowego  ******** */
/* Wy�wietlenie odebranego znaku */
void checkForRcvdData( void )
{
if ( COM_ADDRESS(currentPort) != 0 )
{
   regs.h.ah = 3;                       /* AH = 3 - odczytaj stan portu */
   regs.x.dx = currentPort;                /* DX = numer portu */
   int86( 0x14, &regs, &regs );            /* Wywo�aj funkcj� BIOS */
   if ( regs.h.ah & DataReceived )         /* AH = stan linii */
   {
      regs.h.ah = 2;                    /* AH = 2 - czytaj odebrany znak */
      regs.x.dx = currentPort;             /* DX = numer portu */
      int86( 0x14, &regs, &regs );         /* Wywo�aj funkcj� BIOS */
      writeCharToLineWindow( regs.h.al );  /* AL = znak */
      }
   }
}

/* Wy�lij znak */
unsigned int transmitChar( unsigned char character )
{
regs.h.ah = 1;                         /* AH = 1 - wy�lij znak */
regs.h.al = character;                 /* AL = znak */
regs.x.dx = currentPort;               /* DX = numer portu */
int86( 0x14, &regs, &regs );           /* Wywo�anie funkcji BIOS */
return( regs.h.ah );                   /* AH = stan linii */
}

/* Podaj stan linii i modemu */
unsigned int getComPortStatus( void )
{
regs.h.ah = 3;                         /* AH = 3 - podaj stan */
regs.x.dx = currentPort;               /* DX = numer portu */
int86( 0x14, &regs, &regs );           /* Wywo�aj funkcj� BIOS */
return( regs.x.ax );                   /* AH = stan linii */
                                       /* AL = stan modemu */
}

/* Wy�wietlenie bie��cego stanu linii i modemu */
void showPortStatus( unsigned int row, 
                     unsigned int lsCol, 
                     unsigned int msCol )
{
unsigned int status;

unsigned char lineStatus,
              modemStatus,
              testBit;

status = getComPortStatus();

/* Starsze 8 bit�w == stan linii, m�odsze 8 bit�w == stan modemu */
lineStatus = status >> 8;     /* Odseparuj stan linii */
modemStatus = status;         /* Odseparuj stan modemu */

for( testBit = BIT7; testBit != 0; testBit >>= 1 , row++ )
{
   writeChar( row, lsCol, bitOnOff( lineStatus, testBit ) );
   writeChar( row, msCol, bitOnOff( modemStatus, testBit ) );
   }
}

/* Ustawienie szybko�ci, parzysto�ci, i innych parametr�w
   dla bie��cego portu */
void setComParameters( void )
{
regs.h.ah = 0;                         /* AH = 0 - ustaw parametry */
regs.h.al = cfgWords[currentPort];     /* AL = parametry */
regs.x.dx = currentPort;               /* DX = numer portu */
int86( 0x14, &regs, &regs );           /* Wywo�aj funkcj� BIOS */
}

/* Wy�wietlenie szybko�ci, parzysto�ci itp.
   dla bie��cego portu szeregowego */
void showSettings( void )
{
say( STATROW+0, STATCOL, comPortNames[currentPort] );

if ( COM_ADDRESS(currentPort) == 0 )
	say( STATROW+1, STATCOL, "Brak portu" );
else
{
   sprintf( scratch, "%.4Xh", COM_ADDRESS(currentPort) );
   say( STATROW+1, STATCOL, "          " );
   say( STATROW+1, STATCOL, scratch );
   }

sprintf( scratch, "%.3u", COM_TIMEOUT(currentPort) );
say( STATROW+2, STATCOL, scratch );

switch ( cfgWords[currentPort] & BAUD9600 )
{
   case BAUD110  : say( STATROW+3, STATCOL, "110 " );
                   break;
   case BAUD150  : say( STATROW+3, STATCOL, "150 " );
                   break;
   case BAUD300  : say( STATROW+3, STATCOL, "300 " );
                   break;
   case BAUD600  : say( STATROW+3, STATCOL, "600 " );
                   break;
   case BAUD1200 : say( STATROW+3, STATCOL, "1200" );
                   break;
   case BAUD2400 : say( STATROW+3, STATCOL, "2400" );
                   break;
   case BAUD4800 : say( STATROW+3, STATCOL, "4800" );
                   break;
   case BAUD9600 : say( STATROW+3, STATCOL, "9600" );
                   break;
   }

switch ( cfgWords[currentPort] & EVENPARITY )
{
	case NOPARITY   : say( STATROW+4, STATCOL, "Brak         " );
                     break;
   case ODDPARITY  : say( STATROW+4, STATCOL, "Nieparzysto��" );
                     break;
	case EVENPARITY : say( STATROW+4, STATCOL, "Parzysto��   " );
                     break;
   }

switch ( cfgWords[currentPort] & DATA8 )
{
   case DATA7 : say( STATROW+5, STATCOL, "7" );
                break;
   case DATA8 : say( STATROW+5, STATCOL, "8" );
                break;
   }

switch ( cfgWords[currentPort] & STOP2 )
{
   case STOP1 : say( STATROW+6, STATCOL, "1" );
                break;
   case STOP2 : say( STATROW+6, STATCOL, "2" );
                break;
   }
}
