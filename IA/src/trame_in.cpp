#include "trame_in.h"
#include <assert.h>

#define GET_WORD(a,b) (((a) << 8) | (b))

trame_in_t::trame_in_t() :
  pic1_spare(8,0),
  pic2_spare(39,0),
  pic3_spare(11,0)
{
  
}

void trame_in_t::deserialise(const trame_binary_t & frame)
{
  // PIC 1
  int i=8;
  position_pelle=frame[i];
  mesure_us_1=frame[i+1];
  mesure_us_2=frame[i+2];
  mesure_us_3=frame[i+3];
  bo_basse   =frame[i+4];
  bo_haute   =frame[i+5];

  for(int j=0;j<pic1_spare.size();j++){
    pic1_spare[j]=frame[i+6+j];
  }

  // PIC 2
  i=4+TAILLE_PAQUET_PIC_PC_1+4;

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
  left_pwm_motor   =GET_WORD(frame[i+15],frame[i+16]);


  right_nb_pulse_com=GET_WORD(frame[i+17],frame[i+18]);
  right_nb_pulse_pid=frame[i+19];
  right_error       =frame[i+20];
  right_sum_integ   =GET_WORD(frame[i+21],frame[i+22]);
  right_pwm_motor   =GET_WORD(frame[i+23],frame[i+24]);

  for(int j=0;j<pic2_spare.size();j++){
    pic2_spare[j]=frame[i+j+25];
  }

  // PIC 3
  i=4+TAILLE_PAQUET_PIC_PC_1+TAILLE_PAQUET_PIC_PC_2+4;

  start=frame[i] ;
  unsigned tmp = frame[i+1] & 0x33; // 00110011
  contact=((tmp&0x30)>>2) | (tmp&0x3); // 00001111
  bar_status_init =(frame[i+2] >> 7) & 0xFE;
  bar_pos_courante=GET_WORD(frame[i+3],
			    frame[i+4]);
  bar_nb_pulse_com=GET_WORD(frame[i+5],
			    frame[i+6]);
  bar_nb_pulse_pid=GET_WORD(frame[i+7],
			    frame[i+8]);
  bar_error	  =GET_WORD(frame[i+9],
			    frame[i+10]);
  bar_sum_integ   =GET_WORD(frame[i+11],
			    frame[i+12]);
  bar_pwm         =GET_WORD(frame[i+13],
			    frame[i+14]);
  bar_ir_sensor   =GET_WORD(frame[i+15],
			    frame[i+16]);

  for(int j=0;j<pic3_spare.size();j++){
    pic3_spare[j]=frame[i+j+15];
  }

}
