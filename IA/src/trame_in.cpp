#include "trame_in.h"
#include <assert.h>

#define GET_WORD(a,b) (((a) << 8) || (b))

trame_in_t::trame_in_t() :
  pic1_spare(8,0),
  pic2_spare(39,0),
  pic3_spare(13,0)
{
  
}

void trame_in_t::deserialise(const trame_binary_t & frame)
{
  //frame magic number /!\ normaly previously check in the COM layer !!
  // assert((frame[0]==0x1a)&&
// 	 (frame[1]==0xcf)&&
// 	 (frame[2]==0xfc)&&
// 	 (frame[3]==0x1d));
//   //packet header debug
//   assert((frame[4]==0)&&
// 	 (frame[5]== TAILLE_PAQUET_PIC_PC_1));

  // PIC 1
  int i=8;
  position_pelle=frame[i];
  mesure_us_1=frame[i+1];
  mesure_us_2=frame[i+2];
  mesure_us_3=frame[i+3];
  bo_basse   =frame[i+4];
  bo_haute   =frame[i+5];

  i=14;

  for(int j=0;j<8;j++){
    pic1_spare[j]=frame[i+j];
  }

  // PIC 2
  i=4+TAILLE_PAQUET_PIC_PC_1;
    //
//   assert((frame[i+0]==2)&&
// 	 (frame[i+1]==TAILLE_PAQUET_PIC_PC_2 ));

  i+=4;
  x                =GET_WORD(frame[i+0],frame[i+1]);
  y                =GET_WORD(frame[i+2],frame[i+3]);
  cap              =GET_WORD(frame[i+4],frame[i+5]);
  speed            =frame[i+6];
  current_wp_number=frame[i+7];
  next_wp_number   =frame[i+8];
  
  left_nb_pulse_com=GET_WORD(frame[i+9],frame[i+10]);
  left_nb_pulse_pid=frame[i+11];
  left_error       =frame[i+12];
  left_sum_integ   =GET_WORD(frame[i+13],frame[i+14]);
//   left_current_sense=frame[i+15];
  left_pwm_motor   =GET_WORD(frame[i+15],frame[i+16]);

  i+=17;
  right_nb_pulse_com=GET_WORD(frame[i],frame[i+1]);
  right_nb_pulse_pid=frame[i+2];
  right_error       =frame[i+3];
  right_sum_integ   =GET_WORD(frame[i+4],frame[i+5]);
//   right_current_sense=frame[i+6];
  right_pwm_motor   =GET_WORD(frame[i+6],frame[i+7]);

  i+=9;
  for(int j=0;j<39;j++){
    pic2_spare[j]=frame[i+j];
  }

  // PIC 3
  i=4+TAILLE_PAQUET_PIC_PC_1+TAILLE_PAQUET_PIC_PC_2;
//   assert((frame[i+0]==4)&&
// 	 (frame[i+1]==TAILLE_PAQUET_PIC_PC_3));
  i+=4;
  start=frame[i];
  contact=frame[i+1];
  bar_status_init =(frame[i+2] >> 7) & 0xFE;
  bar_pos_courante=GET_WORD(frame[i+3],frame[i+4]);
  bar_nb_pulse_com=GET_WORD(frame[i+5],frame[i+6]);
  bar_nb_pulse_pid=frame[i+7];
  bar_error	  =frame[i+8];
  bar_sum_integ   =GET_WORD(frame[i+9],frame[i+10]);
  bar_pwm         =GET_WORD(frame[i+11],frame[i+12]);

  i+=13;
  for(int j=0;j<13;j++){
    pic3_spare[j]=frame[i+j];
  }

}
