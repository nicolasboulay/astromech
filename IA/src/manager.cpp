#include <QTextStream>
#include <assert.h>

#include"manager.h"
#include "comportement.h"
#include "comportement_defaut.h"
#include "comportement_dummy.h"
#include "comportement_test.h"
#include "comportement_endofmatch.h"
#include "comportement_navigation.h"
#include "comportement_strategie.h"
#include "comportement_eye.h"

//
// Pour ajouter un comportement
// - créer un comportement_xxx.h et comportement_xxx.cpp
// - déclarer comportement_xxx.h dans manager.cpp (ici)
// - enregister comportement_xxx_t dans le constructeur de manager_t
//   - !!! les valeurs de priorité sont primordials !
// - rajouter les bon numéros dans common.h
// - rajouter les 2 fichiers dans ia.pro... (ça link mieux ensuite...)

using namespace std;
manager_t:: manager_t() 
  :  priority_rank(REFEREE_NUMBER, QVector<int> (10)),
     compo(0),
     res(COMPO_NUMBER)
{
  // comportement that do nothing at the highest priority
  comportement_t * dummy1 = new comportement_dummy_t(COMPO_DUMMY1);
  compo.append(dummy1); 
  setPriority(COMPO_DUMMY1, REFEREE_DEPLACEMENT,0);
  setPriority(COMPO_DUMMY1, REFEREE_TOOLS,0);
  setPriority(COMPO_DUMMY1, REFEREE_DISPLAY,0);
  setPriority(COMPO_DUMMY1, REFEREE_NAVIGATION,0);
  setPriority(COMPO_DUMMY1, REFEREE_GESTION,0);

  comportement_t * nav = new comportement_navigation_t(COMPO_NAVIGATION);
  compo.append( nav);
  setPriority(COMPO_NAVIGATION, REFEREE_DEPLACEMENT,3);
  setPriority(COMPO_NAVIGATION, REFEREE_TOOLS,3);
  setPriority(COMPO_NAVIGATION, REFEREE_DISPLAY,3);
  setPriority(COMPO_NAVIGATION, REFEREE_NAVIGATION,3);
  setPriority(COMPO_NAVIGATION, REFEREE_GESTION,3);

  comportement_t * endofmatch = new comportement_endofmatch_t(COMPO_ENDOFMATCH);
  compo.append(endofmatch); 
  setPriority(COMPO_ENDOFMATCH, REFEREE_DEPLACEMENT,5);
  setPriority(COMPO_ENDOFMATCH, REFEREE_TOOLS,5);
  setPriority(COMPO_ENDOFMATCH, REFEREE_DISPLAY,5);
  setPriority(COMPO_ENDOFMATCH, REFEREE_NAVIGATION,5);
  setPriority(COMPO_ENDOFMATCH, REFEREE_GESTION,5);

  //evitement
  //automate ?

  comportement_t * strategie = new comportement_strategie_t(COMPO_STRATEGIE);
  compo.append(strategie); 
  setPriority(COMPO_ENDOFMATCH, REFEREE_DEPLACEMENT,20);
  setPriority(COMPO_ENDOFMATCH, REFEREE_TOOLS,20);
  setPriority(COMPO_ENDOFMATCH, REFEREE_DISPLAY,20);
  setPriority(COMPO_ENDOFMATCH, REFEREE_NAVIGATION,20);
  setPriority(COMPO_ENDOFMATCH, REFEREE_GESTION,20);

//   comportement_t * eye = new comportement_eye_t(COMPO_EYE);
//   compo.append(eye); 
//   setPriority(COMPO_EYE, REFEREE_DEPLACEMENT,80);
//   setPriority(COMPO_EYE, REFEREE_TOOLS,80);
//   setPriority(COMPO_EYE, REFEREE_DISPLAY,80);
//   setPriority(COMPO_EYE, REFEREE_NAVIGATION,80);
//  setPriority(COMPO_EYE, REFEREE_GESTION,80);

  comportement_t * test = new comportement_test_t(COMPO_TEST);
  compo.append(test); 
  setPriority(COMPO_TEST, REFEREE_DEPLACEMENT,90);
  setPriority(COMPO_TEST, REFEREE_TOOLS,90);
  setPriority(COMPO_TEST, REFEREE_DISPLAY,90);
  setPriority(COMPO_TEST, REFEREE_NAVIGATION,90);
  setPriority(COMPO_TEST, REFEREE_GESTION,90);


  //Lowest priority comportement for "sane" behaviour 
  comportement_t * defaut = new comportement_defaut_t(COMPO_DEFAUT);
  compo.append(defaut); 
  setPriority(COMPO_DEFAUT, REFEREE_DEPLACEMENT,100);
  setPriority(COMPO_DEFAUT, REFEREE_TOOLS,100);
  setPriority(COMPO_DEFAUT, REFEREE_DISPLAY,100);
  setPriority(COMPO_DEFAUT, REFEREE_NAVIGATION,100);
  setPriority(COMPO_DEFAUT, REFEREE_GESTION,100);
}

void manager_t::setPriority(int compo_n, int referee, int rank)
{
  //usefull because the second dimension is not set
  if(priority_rank[referee].size() < rank){
    priority_rank[referee].resize(rank+2);
  }
  priority_rank[referee][rank]=compo_n;
}

trame_out_t & manager_t::execute(const trame_in_t & in,  internal_state_t & state)
{ 
  QTextStream qout(stdout);

  for (int i = 0; i < compo.size(); ++i) {
    res[i]=compo[i]->execute(in,state);
    qout << "execute " <<res[i].name()<< endl;
  }

  QVector<int>  choosed_compo;
  referee(choosed_compo);

  return out;
}

QString manager_t::refereetostring(int r)
{
  switch(r){
  case REFEREE_DEPLACEMENT: return "REFEREE_DEPLACEMENT";
  case REFEREE_TOOLS      : return "REFEREE_TOOLS      ";
  case REFEREE_DISPLAY    : return "REFEREE_DISPLAY    ";
  case REFEREE_NAVIGATION : return "REFEREE_NAVIGATION ";
  case REFEREE_GESTION    : return "REFEREE_GESTION    ";
  case REFEREE_NUMBER     : return "REFEREE_NUMBER     ";
  }
  return("<ERROR>");
}


void manager_t::referee(  QVector<int> & choosed_compo)
{

  QTextStream qout(stdout);
  int i,n; 

  for (n = 0; n < REFEREE_NUMBER; ++n) {
    int n_compo=0;
    for (i = 0; i < priority_rank[n].size(); ++i) {
      n_compo=priority_rank[n][i];
      if(res[n_compo].isActive.at(n)){

	qout << refereetostring(n) << " actif: "<< res[n_compo].name() << ":" 
	     << n_compo << "(priority=" << i << ")"<<endl;
	break;
      }

    }

    assert( n_compo < res.size());
    choosed_compo.append(n_compo);
  }   

  copyTrame(choosed_compo);
}

// Each referee control the access of a part of the output trame.
// the lowest priority comportement are used

void manager_t::copyTrame(QVector<int> choosed_compo)
{
    QTextStream qout(stdout);
  //default value to not forget anything
  //out=res[choosed_compo[REFEREE_GESTION]]; 

      //pic1
  out.pic1_reset = res[choosed_compo[REFEREE_GESTION]].pic1_reset;
  out.led1_jaune = res[choosed_compo[REFEREE_DISPLAY]].led1_jaune;
  out.led1_vert  = res[choosed_compo[REFEREE_DISPLAY]].led1_vert;
  out.led1_orange= res[choosed_compo[REFEREE_DISPLAY]].led1_orange;

  out.pelle_feedback = res[choosed_compo[REFEREE_GESTION]].pelle_feedback;

  out.servo_pelle    =res[choosed_compo[REFEREE_TOOLS]].servo_pelle;
  out.servo_ouverture=res[choosed_compo[REFEREE_TOOLS]].servo_ouverture;
  out.servo_3=res[choosed_compo[REFEREE_TOOLS]].servo_3;

  out.pic1_spare=res[choosed_compo[REFEREE_GESTION]].pic1_spare;
  //pic2
  out.pic2_reset = res[choosed_compo[REFEREE_GESTION]].pic2_reset;
  out.led2_rouge = res[choosed_compo[REFEREE_DISPLAY]].led2_rouge;
  out.led2_orange= res[choosed_compo[REFEREE_DISPLAY]].led2_orange;
  out.led2_jaune = res[choosed_compo[REFEREE_DISPLAY]].led2_jaune;

  out.mode_pwm =res[choosed_compo[REFEREE_DEPLACEMENT]].mode_pwm;
  out.mode_vitesse =res[choosed_compo[REFEREE_DEPLACEMENT]].mode_vitesse;
  out.mode_wp =res[choosed_compo[REFEREE_DEPLACEMENT]].mode_wp;

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
  out.waypoint_cap_x    =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_cap_x;
  out.waypoint_cap_y    =res[choosed_compo[REFEREE_DEPLACEMENT]].waypoint_cap_y;
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


  out.consigne_speed_right=res[choosed_compo[REFEREE_GESTION]].consigne_speed_right;
  out.consigne_speed_left=res[choosed_compo[REFEREE_GESTION]].consigne_speed_left;

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
