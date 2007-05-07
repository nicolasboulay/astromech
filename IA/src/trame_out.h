#ifndef TRAME_OUT_H
#define TRAME_OUT_H
#include "trame_binary.h"
#include <QVector>
#include <QObject>

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
  QVector<uint8_t> pic1_spare;
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

  QVector<uint8_t> waypoint_number;
  QVector<unsigned short> waypoint_pos_x;
  QVector<unsigned short> waypoint_pos_y;
  QVector<unsigned short> waypoint_cap;
  QVector<unsigned short> waypoint_cap_x;
  QVector<unsigned short> waypoint_cap_y;
  QVector<uint8_t> waypoint_speed;
  QVector<int> waypoint_ctrl_wp_nul;
  QVector<int> waypoint_ctrl_wp_next;
  QVector<int> waypoint_ctrl_sens_wp;
  QVector<int> waypoint_ctrl_rot;
  QVector<int> waypoint_ctrl_sens_rot;
  QVector<int> waypoint_ctrl_pwm;

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

  QVector<uint8_t> pic2_spare; 

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
  unsigned int bar_vitesse_rot_max;
  uint8_t bar_accel_max;
  short bar_pwm;

  int8_t bar_gain_prop;
  int8_t bar_gain_integ;
  int8_t bar_gain_deriv;
  short bar_satur_sum_integ;
  uint8_t bar_thres_prop_only;
  
  QVector<uint8_t> pic3_spare; 
  void serialise( trame_binary_t & frame);
};
#endif
