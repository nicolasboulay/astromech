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
  pic1_spare(11,0),

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

  waypoint_number(2,0),
  waypoint_pos_x(2,0),
  waypoint_pos_y(2,0),
  waypoint_cap(2,0),
  waypoint_cap_x(2,0),
  waypoint_cap_y(2,0),
  waypoint_speed(2,0),

  waypoint_ctrl_wp_nul(2,0),
  waypoint_ctrl_wp_next(2,0),
  waypoint_ctrl_sens_wp(2,0),
  waypoint_ctrl_rot(2,0),
  waypoint_ctrl_sens_rot(2,0),
  waypoint_ctrl_pwm(2,0),

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
  pic3_spare(6,0)
{
  waypoint_number[0]=0;
  waypoint_number[1]=0;
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


  for(int j=0;j<11;j++){
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

  frame[i+12]=waypoint_number[0];
  frame[i+13]=GET_H(waypoint_pos_x.at(0));
  frame[i+14]=GET_L(waypoint_pos_x.at(0));
  frame[i+15]=GET_H(waypoint_pos_y.at(0));
  frame[i+16]=GET_L(waypoint_pos_y.at(0));
  frame[i+17]=GET_H(waypoint_cap.at(0));
  frame[i+18]=GET_L(waypoint_cap.at(0));
  frame[i+19]=GET_H(waypoint_cap_x.at(0));
  frame[i+20]=GET_L(waypoint_cap_x.at(0));
  frame[i+21]=GET_H(waypoint_cap_y.at(0));
  frame[i+22]=GET_L(waypoint_cap_y.at(0));
  frame[i+23]=waypoint_speed.at(0);
  uint8_t waypoint_ctrl0 = 
    ((waypoint_ctrl_wp_nul.at(0)&0x1)<<7) |
    ((waypoint_ctrl_wp_next.at(0)&0x1)<<6) |
    ((waypoint_ctrl_sens_wp.at(0)&0x1)<<5) |
    ((waypoint_ctrl_rot.at(0)&0x1)<<4) |
    (( waypoint_ctrl_sens_rot.at(0)&0x1)<<3) |
    ((waypoint_ctrl_pwm.at(0)&0x1)<<2);
  frame[i+24]=waypoint_ctrl0;

  frame[i+25]=waypoint_number[1];
  frame[i+26]=GET_H(waypoint_pos_x.at(1));
  frame[i+27]=GET_L(waypoint_pos_x.at(1));
  frame[i+28]=GET_H(waypoint_pos_y.at(1));
  frame[i+29]=GET_L(waypoint_pos_y.at(1));
  frame[i+30]=GET_H(waypoint_cap.at(1));
  frame[i+31]=GET_L(waypoint_cap.at(1));
  frame[i+32]=GET_H(waypoint_cap_x.at(1));
  frame[i+33]=GET_L(waypoint_cap_x.at(1));
  frame[i+34]=GET_H(waypoint_cap_y.at(1));
  frame[i+35]=GET_L(waypoint_cap_y.at(1));
  frame[i+36]=waypoint_speed.at(1);
  uint8_t waypoint_ctrl1 = 
    ((waypoint_ctrl_wp_nul.at(1)&0x1)<<7) |
    ((waypoint_ctrl_wp_next.at(1)&0x1)<<6) |
    ((waypoint_ctrl_sens_wp.at(1)&0x1)<<5) |
    ((waypoint_ctrl_rot.at(1)&0x1)<<4) |
    (( waypoint_ctrl_sens_rot.at(1)&0x1)<<3) |
    ((waypoint_ctrl_pwm.at(1)&0x1)<<2);
  frame[i+37]=waypoint_ctrl1;

  frame[i+38]=GET_H(right_pwm_motor);
  frame[i+39]=GET_L(right_pwm_motor);
  frame[i+40]=GET_H(left_pwm_motor);
  frame[i+41]=GET_L(left_pwm_motor);
  frame[i+42]=left_gain_prop;
  frame[i+43]=left_gain_integ;
  frame[i+44]=left_gain_deriv;
  frame[i+45]=GET_H(left_satur_sum_integ);
  frame[i+46]=GET_L(left_satur_sum_integ);
  frame[i+47]=left_thres_prop_only;

  frame[i+48]=right_gain_prop;
  frame[i+49]=right_gain_integ;
  frame[i+50]=right_gain_deriv;
  frame[i+51]=GET_H(right_satur_sum_integ);
  frame[i+52]=GET_L(right_satur_sum_integ);
  frame[i+53]=right_thres_prop_only;
  
  for(int j=0;j<17;j++){
    frame[i+j]=pic2_spare[j];
  }
  //3
  i=4+TAILLE_PAQUET_PC_PIC_1+TAILLE_PAQUET_PC_PIC_2+3; 
  unsigned short pic3_ctrl = 
    ((pic3_reset&0x1)<<7) |
    ((led3_vert&0x1)<<6) |
    ((led3_jaune&0x1)<<5) |
    ((led3_orange&0x1)<<4) |
    ((led3_rouge&0x1)<<3);
  frame[i]=pic3_ctrl;

  unsigned short bar_ctrl = 
    ((ctrl_bar_sens_rot &0x1)<<7) |
    ((ctrl_bar_pwm&0x1)<<6) |
    ((ctrl_bar_libre&0x1)<<5) |
    ((ctrl_bar_init&0x1)<<4);
  frame[i+1]=bar_ctrl;
  
  frame[i+2]=GET_H(bar_consigne_pos);
  frame[i+3]=GET_L(bar_consigne_pos);
  frame[i+4]=GET_H(bar_vitesse_rot_max);
  frame[i+5]=GET_L(bar_vitesse_rot_max);
  frame[i+6]=bar_accel_max;
  frame[i+7]=GET_H(bar_pwm);
  frame[i+8]=GET_L(bar_pwm);

  frame[i+9]=bar_gain_prop;
  frame[i+10]=bar_gain_integ;
  frame[i+11]=bar_gain_deriv;
  frame[i+12]=GET_H(bar_satur_sum_integ);
  frame[i+13]=GET_L(bar_satur_sum_integ);
  frame[i+14]=bar_thres_prop_only;
  
  for(int j=0;j<6;j++){
    frame[i+j]=pic3_spare[j];
  }

}
