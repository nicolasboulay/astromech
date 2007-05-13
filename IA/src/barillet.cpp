#include "barillet.h"
#include <cmath>

bool  barillet_t ::initPosition(const trame_in_t & in, trame_out_t & out)
{

  out.ctrl_bar_init=1;

  if(in.bar_status_init==0){
    return false;
  }
    return true;
}

unsigned int degres_to_consigne(double degree)
{
  // 42 000 impulsion par tour
  
  return static_cast<unsigned int> ((degree*360.0)/42000.0);
}

bool barillet_t ::set(const double & degres, enum sensRot_t sensrot, 
	 const trame_in_t & in, trame_out_t & out)
{
  
  out.bar_consigne_pos = degres_to_consigne(degres);
  if (sensrot != KEEP){
    out.ctrl_bar_sens_rot= sensrot;
  }

  if(fabs(in.bar_pos_courante - degres) > degres_to_consigne(2.0)) {
    return false;
  }
  return true;
}

//valeur par défaut
bool barillet_t ::todefault(const trame_in_t & in, trame_out_t & out)
{

  out.ctrl_bar_sens_rot=1;//rotation par default: celui qui n'arrache pas les capteurs 
  out.ctrl_bar_pwm  =0; // mode asservis
  out.ctrl_bar_libre=0; // maintent de la position
  out.ctrl_bar_init =0; // pas d'init de position

  //out.bar_consigne_pos; // on garde la valeur d'avant
  out.bar_vitesse_rot_max = 400; // vitesse max
  out.bar_accel_max       = 10;  // acceleration max
  out.bar_pwm             =  0;  // pas utilisé, uniquement en mode pwm

  //pid : à régler !
  out.bar_gain_prop       = 1;  
  out.bar_gain_integ      = 0;
  out.bar_gain_deriv      = 0;
  out.bar_satur_sum_integ = 100;
  out.bar_thres_prop_only = 200;

  return true;
}

//exemple de séquence
bool barillet_t ::test_init()
{
  test_step=0;
  return true;
}
bool barillet_t ::test(const trame_in_t & in, trame_out_t & out)
{
  todefault(in,out);
  
  switch(test_step){
  case 0 : if(initPosition(in,out)) test_step++; break;
  case 1 : if(set(0.0,KEEP,in,out)) test_step++; break;
  case 2 : if(set(90.0,KEEP,in,out)) test_step++; break;
  case 3 : if(set(180.0,KEEP,in,out)) test_step++; break;
  case 4 : if(set(270.0,KEEP,in,out)) test_step++; break;
  case 5 : return true;
  }
  return false;
}
