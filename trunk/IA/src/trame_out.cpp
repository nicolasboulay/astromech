#include "trame_out.h"

#define GET_WORD(a,b) (((a) << 8) |  (b))
#define GET_H(a) (a>>8)
#define GET_L(a) (a & 0xff)

trame_out_t::trame_out_t() :
  pic1_reset(0),
  led1_jaune(0),
  led1_vert(0),
  led1_orange(0),
  pelle_feedback(0),
  servo_pelle (0),
  servo_ouverture(0),
  servo_3(0),
  servo_4(0),
  servo_5(0),
  servo_6(0),
  pic1_spare(16,0),
  pic2_reset(0),
  led2_rouge(0),
  led2_orange(0),
  led2_jaune(0),
  nav_ctrl_update(0),
  new_position_x(0),
  new_position_y(0),
  new_cap(0),
  quat_w(0),
  quat_z(0),

  waypoint_number(0),
  waypoint_pos_x(0),
  waypoint_pos_y(0),
  waypoint_cap(0),
  waypoint_speed(0),
  waypoint_ctrl_wp_nul(0),
  waypoint_ctrl_wp_next(0),
  waypoint_ctrl_sens_wp(0),
  waypoint_ctrl_rot(0),
  waypoint_ctrl_sens_rot(0),
  waypoint_ctrl_pwm(0),
  right_pwm_motor(0),
  left_pwm_motor(0),

  left_gain_prop(0),
  left_gain_integ(0),
  left_gain_deriv(0),
  left_satur_sum_integ(0),
  left_thres_prop_only(0),

  right_gain_prop(0),
  right_gain_integ(0),
  right_gain_deriv(0),
  right_satur_sum_integ(0),
  right_thres_prop_only(0),
  pic2_spare(17,0),

  pic3_reset(0),
  led3_vert(0),
  led3_jaune(0),
  led3_orange(0),
  led3_rouge(0),

  ctrl_bar_sens_rot(0),
  ctrl_bar_pwm(0),
  ctrl_bar_libre(0),
  ctrl_bar_init(0),

  bar_consigne_pos(0),
  bar_vitesse_rot_max(0),
  bar_accel_max(0),
  bar_pwm(0),

  bar_gain_prop(0),
  bar_gain_integ(0),
  bar_gain_deriv(0),
  bar_satur_sum_integ(0),
  bar_thres_prop_only(0),
  
  pic3_spare(17,0)
{

}

void trame_out_t::serialise(trame_binary_t & frame)
{
  int i;
  unsigned short pic_ctrl = 
    ((pic1_reset&0x1)<<7) |
    ((led1_jaune&0x1)<<6) |
    ((led1_vert&0x1)<<5) |
    ((led1_orange&0x1)<<4) |
    ((pelle_feedback&0x1)<<3);
  //1
  frame[7]=pic_ctrl;
  frame[8]= servo_pelle ;
  frame[9]= servo_ouverture;
  frame[10]= servo_3;
  frame[11]= servo_4;
  frame[12]= servo_5;
  frame[13]= servo_6;

  for(int j=0;j<16;j++){
    frame[14+j]=pic1_spare[j];
  }
  //2
  i=4+TAILLE_PAQUET_PC_PIC_1+3; 
  unsigned short pic2_ctrl = 
    ((pic2_reset&0x1)<<7) |
    ((led2_rouge&0x1)<<6) |
    ((led2_orange&0x1)<<5) |
    ((led2_jaune&0x1)<<4);
  frame[i]=pic2_ctrl;
  frame[i+1]=(nav_ctrl_update & 0x1)<< 7;
  frame[i+2]=GET_H(new_position_x);
  frame[i+3]=GET_L(new_position_x);
  frame[i+4]=GET_H(new_position_y);
  frame[i+5]=GET_L(new_position_y);
  frame[i+6]=GET_H(new_cap);
  frame[i+7]=GET_L(new_cap);
  frame[i+8]=GET_H(quat_w);
  frame[i+9]=GET_L(quat_w);
  frame[i+10]=GET_H(quat_z);
  frame[i+11]=GET_L(quat_z);


  //3
  i=4+TAILLE_PAQUET_PC_PIC_1+TAILLE_PAQUET_PC_PIC_2+3; 
  unsigned short pic3_ctrl = 
    ((pic3_reset&0x1)<<7) |
    ((led3_vert&0x1)<<6) |
    ((led3_jaune&0x1)<<5) |
    ((led3_orange&0x1)<<4) |
    ((led3_rouge&0x1)<<3);
  frame[i]=pic3_ctrl;
  
}
