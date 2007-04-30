#ifndef TRAME_OUT_H
#define TRAME_OUT_H
#include "trame_binary.h"

//
// Définition logique de la trame PC ->pics
//

class trame_out_t 
{

  public :
  trame_out_t();
    //pic1
  int pic1_reset;
  int led1_jaune;
  int led1_vert;
  int led1_orange;
  int pelle_feedback;
  unsigned short  servo_pelle ;
  unsigned short  servo_ouverture;
  unsigned short  servo_3;
  unsigned short  servo_4;
  unsigned short  servo_5;
  unsigned short  servo_6;
  QVector<unsigned short> pic1_spare;
  //pic2
  int pic2_reset;
  int led2_rouge;
  int led2_orange;
  int led2_jaune;
  int nav_ctrl_update;
  unsigned int new_position_x;
  unsigned int new_position_y;
  unsigned int new_cap;
  int quat_w;
  int quat_z;

  unsigned short waypoint_number;
  unsigned short waypoint_pos_x;
  unsigned short waypoint_pos_y;
  unsigned short waypoint_cap;
  unsigned short waypoint_speed;
  int waypoint_ctrl_wp_nul;
  int waypoint_ctrl_wp_next;
  int waypoint_ctrl_sens_wp;
  int waypoint_ctrl_rot;
  int waypoint_ctrl_sens_rot;
  int waypoint_ctrl_pwm;
  int right_pwm_motor;
  int left_pwm_motor;

  int left_gain_prop;
  int left_gain_integ;
  int left_gain_deriv;
  int left_satur_sum_integ;
  int left_thres_prop_only;

  int right_gain_prop;
  int right_gain_integ;
  int right_gain_deriv;
  int right_satur_sum_integ;
  int right_thres_prop_only;

  QVector<unsigned short> pic2_spare; 

  //pic3
  int pic3_reset;
  int led3_vert;
  int led3_jaune;
  int led3_orange;
  int led3_rouge;

  int ctrl_bar_sens_rot;
  int ctrl_bar_pwm;
  int ctrl_bar_libre;
  int ctrl_bar_init;

  unsigned int bar_consigne_pos;
  unsigned short bar_vitesse_rot_max;
  unsigned short bar_accel_max;
  short bar_pwm;

  short bar_gain_prop;
  short bar_gain_integ;
  short bar_gain_deriv;
  short bar_satur_sum_integ;
  short bar_thres_prop_only;
  
  QVector<unsigned short> pic3_spare; 
  void serialise( trame_binary_t & frame);
};
#endif
