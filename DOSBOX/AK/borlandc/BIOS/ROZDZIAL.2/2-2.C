/*
   BIOS dla programuj�cych w C i C++
   Copyright (c) 1992 - Billy P. Taylor

   Program 2-2.  U�ycie FP_SEG i FP_OFF ze sta�ymi adresami
                 (tylko Borland C)
*/

char string[] = "B��d drukarki!";
unsigned int segment, offset;

segment = FP_SEG( string );
offset  = FP_OFF( string );
