#include "serial.h"
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <QVector>
#include <iostream>

using namespace std;

serial_t::serial_t(const char * device): frame(TAILLE_TRAME)
{
  error=0;
  TRACE;
  if (0 > (fdr = open(device, O_RDONLY | O_NOCTTY )) ) {
    perror("ouverture port série r");
    error=1;
  } 

  if (0 > (fdw = open(device, O_WRONLY | O_NOCTTY | O_NONBLOCK)) ) {
    perror("ouverture port série w");
    error=1;
  } 
  tcgetattr(fdr,&oldtio); /* save current port settings */ 
  bzero(&newtio, sizeof(newtio));

  //  newtio.c_cflag = B115200| CS8 | CLOCAL | CREAD | CSTOPB;
  newtio.c_cflag = B9600| CS8 | CLOCAL | CREAD | CSTOPB;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  
  newtio.c_lflag = 0; 
  
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = TAILLE_TRAME;   /* blocking read until n chars received */
  
  tcflush(fdr, TCIFLUSH); // flush input buffer
  tcsetattr(fdr,TCSANOW,&newtio); 
  tcflush(fdw, TCIFLUSH);// flush output buffer
  tcsetattr(fdw,TCSANOW,&newtio); 

  index=0;
  bank=0;

  for(int i=0;i<TAILLE_TRAME;i++){
    buffer[0][i]=0xAA;
    buffer[1][i]=0x55;
  }

}

serial_t::~serial_t(){
  tcsetattr(fdr,TCSANOW,&oldtio);
  tcsetattr(fdw,TCSANOW,&oldtio);
  close(fdw);
  close(fdr);

}



void serial_t::exchange(QVector<unsigned char> & in, trame_binary_t & out)
{  
  int n; static int c=0;
  printf("%i\n",c++);

  if ((n=write(fdw, out.data(), out.count())) < 1){
    fprintf (stderr, "Internal error: "
		 "serial write "
		 "%d at %s, line %d.\n",
		 n, __FILE__, __LINE__);
  }

   printf("PC->PIC[%i]:\n",n);

  bank=(bank+1)%2;
  if((n=read(fdr,buffer[bank],TAILLE_TRAME))<1){
    fprintf (stderr, "Internal error: "
	     "serial read "
	     "%d at %s, line %d.\n",
	     n, __FILE__, __LINE__);
  }

      printf("buffer[bank][%i]:\n",n);
      for(int i=0;i<in.count();i++){
        printf("%.2x ", buffer[0][i]);
      }
      printf("\n");
      for(int i=0;i<in.count();i++){
        printf("%.2x ", buffer[1][i]);
      }
      printf("\nbank=%i\n",bank);
   
  find_trame((bank+1)%2);

  in=frame;

}

void serial_t::find_trame(unsigned previous_bank){
  int start=-1;  
  TRACE;
  for(int i=0;i<TAILLE_TRAME;i++){
    if((buffer[previous_bank][i] == 0x1a) &&
       (buffer[previous_bank][i+1] == 0xcf) &&
       (buffer[previous_bank][i+2] == 0xfc) &&
       (buffer[previous_bank][i+3] == 0x1d) ){
      start = i;
      break;
    }
  }
  //  printf("start=%i\n",start);
  
  int j=0;
  if(start != -1){
    for(int i=start;i<TAILLE_TRAME;i++,j++){
      frame[j]=buffer[previous_bank][i];
    }
    buffer[previous_bank][start]=0;
  } else {
    start=TAILLE_TRAME;
  }
  //i<start is not a mistake
  for(int i=0 ;i<start;i++,j++){
    frame[j]=buffer[bank][i];
  } 
  buffer[bank][0]=0;
  TRACE;
}

