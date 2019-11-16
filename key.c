#include <curses.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>
#include </home/martin/Impartial/code/martin.h>

int main(int argc,char *argv[]) 
{ 
    char in[255],pressed,stage;
    int go;


    go=1;
    while ( go == 1 )
    {
initscr();
timeout(1000);
pressed= getch();
endwin();
printf("Char: %c\n", pressed);
      switch(pressed) {
      case 'q' :
         go=0;
	 break;
      case '1' :
         printf("Stage 1\n" ); stage='1';
	 break;
      case '2' :
         printf("Stage 2\n" ); stage='2';
	 break;
      case '3' :
         printf("Stage 3\n" ); stage='3';
	 break;
      case '4' :
         printf("Stage 3\n" ); stage='4';
	 break;
      default :
         printf("Invalid control\n" );
	 break;
      }
   }
}
