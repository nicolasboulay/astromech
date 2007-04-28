#ifndef TRAME_IN_H
#define TRAME_IN_H
#include "trame_binary.h"

//
// Définition logique de la trame pics->PC
//

class trame_in_t 
{

  public :
  unsigned spare[3][8];
    //1
  int position_pelle;
  int mesure_us_1;  //différents échos renvoyés par un capteur ultrasons
  unsigned mesure_us_2;
  unsigned mesure_us_3;
  unsigned bo_basse;
  unsigned bo_haute;
  unsigned pic1_spare[14];
  //2
  int x;
  int y;
  int cap;
  unsigned speed;
  unsigned current_wp_number;
  unsigned next_wp_number;

  unsigned left_nb_pulse_com;
  unsigned left_nb_pusle_pid;
  unsigned left_error;
  unsigned left_sum_integ;
  unsigned left_current_sense;
  int      left_pwm_motor;

  unsigned right_nb_pulse_com;
  unsigned right_nb_pusle_pid;
  unsigned right_error;
  unsigned right_sum_integ;
  unsigned right_current_sense;
  int      right_pwm_motor;
  unsigned pic2_spare[29];
  //3
  unsigned bar_status_init ;
  unsigned bar_pos_courante;
  unsigned bar_nb_pulse_com;
  unsigned bar_nb_pulse_pid;
  int      bar_error;
  unsigned bar_sum_integ;
  int      bar_pwm;
  unsigned pic3_spare[17];


  //extract the data part of a input frame
  void deserialise(const trame_binary_t & frame);
};
#endif
