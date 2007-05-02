#include <iostream>
#include"manager.h"
#include"comportement.h"


manager_t:: manager_t() 
  :  priority_rank(REFEREE_NUMBER, QVector<int> (10)),
     compo(0),
     res(COMPO_NUMBER)
{
  dummy_comportement_t dummy1(COMPO_DUMMY1);
  compo.append(dummy1); 
  setPriority(COMPO_DUMMY1, REFEREE_DEPLACEMENT,0);
  setPriority(COMPO_DUMMY1, REFEREE_TOOLS,0);
  setPriority(COMPO_DUMMY1, REFEREE_DISPLAY,0);
  setPriority(COMPO_DUMMY1, REFEREE_NAVIGATION,0);

  dummy_comportement_t dummy2(COMPO_DUMMY2);
  compo.append(dummy2); 
  setPriority(COMPO_DUMMY2, REFEREE_DEPLACEMENT,1);
  setPriority(COMPO_DUMMY2, REFEREE_TOOLS,1);
  setPriority(COMPO_DUMMY2, REFEREE_DISPLAY,1);
  setPriority(COMPO_DUMMY2, REFEREE_NAVIGATION,1);

}

void manager_t::setPriority(int compo_n, int referee, int rank)
{
  //usefull because the second dimension is not 
  if(priority_rank[referee].size() < rank){
    priority_rank[referee].resize(rank+2);
  }
  priority_rank[referee][rank]=compo_n;
}

trame_out_t manager_t::execute(trame_in_t in)
{ 
  //  TRACE;    printf(":%i\n",compo.size());
  for (int i = 0; i < compo.size(); ++i) {
    res[i]=compo[i].execute(in,state);
    //printf(":%i\n",i);
  }
  //  TRACE;
  generateTrameout();
  //TRACE;
  return out;
}

void manager_t::generateTrameout()
{
  QVector<int> choosed_compo; 
  int i,n; 
  //TRACE;
  for (n = 0; n < REFEREE_NUMBER; ++n) {
    for (i = 0; i < res.size(); ++i) {
      //TRACE;
      if(res[i].isActive.at(n)){
	//printf("%i %i\n",i,n);
	break;
      }
    }
    // TRACE;
    choosed_compo.append(i);
  }   
  copyTrame(choosed_compo);
}

// Each referee control the access of a part of the output trame.
// the lowest priority comportement are used

void manager_t::copyTrame(QVector<int> choosed_compo)
{
  
  //default value to not forget anything
  out=res[choosed_compo[REFEREE_GESTION]]; 

      //pic1
  out.pic1_reset = res[choosed_compo[REFEREE_GESTION]].pic1_reset;
  out.led1_jaune = res[choosed_compo[REFEREE_DISPLAY]].led1_jaune;
  out.led1_vert  = res[choosed_compo[REFEREE_DISPLAY]].led1_vert;
  out.led1_orange= res[choosed_compo[REFEREE_DISPLAY]].led1_orange;

  out.pelle_feedback = res[choosed_compo[REFEREE_GESTION]].pelle_feedback;

  out.servo_pelle    =res[choosed_compo[REFEREE_TOOLS]].servo_pelle;
  out.servo_ouverture=res[choosed_compo[REFEREE_TOOLS]].servo_ouverture;
  out.servo_3=res[choosed_compo[REFEREE_TOOLS]].servo_3;
  out.servo_4=res[choosed_compo[REFEREE_TOOLS]].servo_4;
  out.servo_5=res[choosed_compo[REFEREE_TOOLS]].servo_5;
  out.servo_6=res[choosed_compo[REFEREE_TOOLS]].servo_6;

  out.pic1_spare=res[choosed_compo[REFEREE_GESTION]].pic1_spare;
  //pic2
  out.pic2_reset = res[choosed_compo[REFEREE_GESTION]].pic2_reset;
  out.led2_rouge = res[choosed_compo[REFEREE_DISPLAY]].led2_rouge;
  out.led2_orange= res[choosed_compo[REFEREE_DISPLAY]].led2_orange;
  out.led2_jaune = res[choosed_compo[REFEREE_DISPLAY]].led2_jaune;

  out.nav_ctrl_update= res[choosed_compo[REFEREE_NAVIGATION]].nav_ctrl_update;
  out.new_position_x = res[choosed_compo[REFEREE_NAVIGATION]].new_position_x;
  out.new_position_y = res[choosed_compo[REFEREE_NAVIGATION]].new_position_y;
  out.new_cap= res[choosed_compo[REFEREE_NAVIGATION]].new_cap;
  out.quat_w = res[choosed_compo[REFEREE_NAVIGATION]].quat_w;
  out.quat_z = res[choosed_compo[REFEREE_NAVIGATION]].quat_z;

  out.waypoint_number =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_number;
  out.waypoint_pos_x  =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_pos_x;
  out.waypoint_pos_y  =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_pos_y;
  out.waypoint_cap    =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_cap;
  out.waypoint_speed  =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_speed;
  out.waypoint_ctrl_wp_nul =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_wp_nul;
  out.waypoint_ctrl_wp_next=res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_wp_next;
  out.waypoint_ctrl_sens_wp=res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_sens_wp;
  out.waypoint_ctrl_rot    =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_rot;
  out.waypoint_ctrl_sens_rot=res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_sens_rot;
  out.waypoint_ctrl_pwm     =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_ctrl_pwm;

  out.right_pwm_motor=res[choosed_compo[REFEREE_GESTION]].right_pwm_motor;
  out.left_pwm_motor =res[choosed_compo[REFEREE_GESTION]].left_pwm_motor;
  out.left_gain_prop =res[choosed_compo[REFEREE_GESTION]].left_gain_prop;
  out.left_gain_integ=res[choosed_compo[REFEREE_GESTION]].left_gain_integ;
  out.left_gain_deriv=res[choosed_compo[REFEREE_GESTION]].left_gain_deriv;
  out.left_satur_sum_integ=res[choosed_compo[REFEREE_GESTION]].left_satur_sum_integ;
  out.left_thres_prop_only=res[choosed_compo[REFEREE_GESTION]].left_thres_prop_only;
  out.right_gain_prop =res[choosed_compo[REFEREE_GESTION]].right_gain_prop;
  out.right_gain_integ=res[choosed_compo[REFEREE_GESTION]].right_gain_integ;
  out.right_gain_deriv=res[choosed_compo[REFEREE_GESTION]].right_gain_deriv;
  out.right_satur_sum_integ=res[choosed_compo[REFEREE_GESTION]].right_satur_sum_integ;
  out.right_thres_prop_only=res[choosed_compo[REFEREE_GESTION]].right_thres_prop_only;

  out.pic2_spare=res[choosed_compo[REFEREE_GESTION]].pic2_spare;

  //pic3
  out.pic3_reset= res[choosed_compo[REFEREE_GESTION]].pic3_reset;
  out.led3_vert = res[choosed_compo[REFEREE_DISPLAY]].led3_vert;
  out.led3_jaune = res[choosed_compo[REFEREE_DISPLAY]].led3_jaune;
  out.led3_orange= res[choosed_compo[REFEREE_DISPLAY]].led3_orange;
  out.led3_rouge=res[choosed_compo[REFEREE_DISPLAY]].led3_rouge;

  out.ctrl_bar_sens_rot=res[choosed_compo[REFEREE_TOOLS]].ctrl_bar_sens_rot;
  out.ctrl_bar_pwm  =res[choosed_compo[REFEREE_TOOLS]].ctrl_bar_pwm;
  out.ctrl_bar_libre=res[choosed_compo[REFEREE_TOOLS]].ctrl_bar_libre;
  out.ctrl_bar_init =res[choosed_compo[REFEREE_TOOLS]].ctrl_bar_init;

  out.bar_consigne_pos=res[choosed_compo[REFEREE_TOOLS]].bar_consigne_pos;
  out.bar_vitesse_rot_max=res[choosed_compo[REFEREE_TOOLS]].bar_vitesse_rot_max;
  out.bar_accel_max =res[choosed_compo[REFEREE_TOOLS]].bar_accel_max;
  out.bar_pwm       =res[choosed_compo[REFEREE_TOOLS]].bar_pwm;

  out.bar_gain_prop =res[choosed_compo[REFEREE_GESTION]].bar_gain_prop;
  out.bar_gain_integ=res[choosed_compo[REFEREE_GESTION]].bar_gain_integ;
  out.bar_gain_deriv=res[choosed_compo[REFEREE_GESTION]].bar_gain_deriv;
  out.bar_satur_sum_integ=res[choosed_compo[REFEREE_GESTION]].bar_satur_sum_integ;
  out.bar_thres_prop_only=res[choosed_compo[REFEREE_GESTION]].bar_thres_prop_only;
  
  out.pic3_spare=res[choosed_compo[REFEREE_GESTION]].pic3_spare;
}
