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

unsigned short degres_to_consigne(double degree)
{
  // 42 000 impulsions par tour (84*500)
  
  return static_cast<unsigned int> ((degree*42000.0)/360.0);
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

int ecartPosition ( int pos1, int pos2, int modulo) {
	int ecart12 = pos2 - pos1;
	int ecart21 = pos1 - pos2;
	
	while ( ecart12 >= modulo ) { ecart12 -= modulo; }
	while ( ecart12 < 0 ) { ecart12 += modulo; }
	while ( ecart21 >= modulo ) { ecart21 -= modulo; }
	while ( ecart21 < 0 ) { ecart21 += modulo; }
	
	if ( ecart12 < ecart21 ) {
		return ecart12;
	} else {
		return ecart21;
	}
	
}

bool barillet_t ::positionneEntreeCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out) {

	// Ecriture de la valeur en out
	if ( num_case >= 0 && num_case <= 8 ) {
		out.bar_consigne_pos = positionsEntree[num_case];
	} else {
		printf("La position d'entree demandee (%d) pour le barillet est en dehors des bornes\n", num_case);
		return false;
	}
	
	// Lecture de la valeur en in
	// Erreur tolérée de 2 degrés
	if ( ecartPosition(in.bar_pos_courante, out.bar_consigne_pos, 42000) < 2/360*42000 ) {
		return true;
	} else {
		return false;
	}
			      
}

bool barillet_t ::positionneSortieCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out) {

	// Ecriture de la valeur en out
	if ( num_case >= 0 && num_case <= 8 ) {
		out.bar_consigne_pos = positionsSortie[num_case];
	} else {
		printf("La position de sortie demandee (%d) pour le barillet est en dehors des bornes\n", num_case);
		return false;
	}
	
	// Lecture de la valeur en in
	// Erreur tolérée de 2 degrés
	if ( ecartPosition(in.bar_pos_courante, out.bar_consigne_pos, 42000) < 2/360*42000 ) {
		return true;
	} else {
		return false;
	}
			      
}

//valeur par défaut
bool barillet_t ::todefault(const trame_in_t & in, trame_out_t & out)
{

  out.ctrl_bar_sens_rot=1;//rotation par default: celui qui n'arrache pas les capteurs 
  out.ctrl_bar_pwm  =1; // mode asservis
  out.ctrl_bar_libre=0; // maintent de la position
  out.ctrl_bar_init =0; // pas d'init de position

  //out.bar_consigne_pos; // on garde la valeur d'avant
  out.bar_consigne_pos=0;
  out.bar_vitesse_rot_max = 200; // vitesse max
  out.bar_accel_max       = 10;  // acceleration max
  out.bar_pwm             =  0;  // pas utilisé, uniquement en mode pwm

  //pid : à régler !
  out.bar_gain_prop       = 8;  
  out.bar_gain_integ      = 1;
  out.bar_gain_deriv      = 0;
  out.bar_satur_sum_integ = 400;
  out.bar_thres_prop_only = 30;

  return true;
}

bool barillet_t ::mode_pwm(const trame_in_t & in, trame_out_t & out)
{
  todefault(in,out);
  out.ctrl_bar_sens_rot=1;//rotation par default: celui qui n'arrache pas les capteurs 
  out.ctrl_bar_pwm  =1; 
  out.ctrl_bar_libre=0; // maintent de la position
  out.ctrl_bar_init =0; // pas d'init de position

  return true;
}

bool barillet_t ::mode_pwm(short pwm, int sens, const trame_in_t & in, trame_out_t & out)
{
  printf("BAR: set pwm %i(%i)\n",pwm,sens);
  mode_pwm(in,out);
  out.bar_pwm=pwm;
  out.ctrl_bar_sens_rot=sens;
  return true;
}

bool barillet_t ::mode_angle(unsigned short pos, int sens, const trame_in_t & in, trame_out_t & out)
{
  printf("BAR: commande:%i tick\n",pos);
  out.ctrl_bar_pwm  =0; // mode asservis
  out.ctrl_bar_sens_rot=sens;
  out.bar_consigne_pos=pos;
  return true;
}

bool barillet_t ::mode_angle(double pos_degree, int sens, const trame_in_t & in, trame_out_t & out)
{
  printf("BAR: commande:%f°\n",pos_degree);
  mode_angle( degres_to_consigne(pos_degree),sens,in,out);
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
  
  printf("Barillet Test Step : %d\n", test_step);
  
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

bool barillet_t ::initialiserPositions()
{
	// Purge des fichiers actuels
	positionsEntree.clear();
	positionsSortie.clear();
	
	// Inscriptions des coefficients
	positionsEntree[0] = 0; // CALIBRAGE
	positionsEntree[1] = 0;
	positionsEntree[2] = 0;
	positionsEntree[3] = 0;
	positionsEntree[4] = 0;
	positionsEntree[5] = 0;
	positionsEntree[6] = 0;
	positionsEntree[7] = 0;
	positionsEntree[8] = 0;
	
	positionsSortie[0] = 0; // CALIBRAGE
	positionsSortie[1] = 0;
	positionsSortie[2] = 0;
	positionsSortie[3] = 0;
	positionsSortie[4] = 0;
	positionsSortie[5] = 0;
	positionsSortie[6] = 0;
	positionsSortie[7] = 0;
	positionsSortie[8] = 0;
	
	return true;
	
} 

void barillet_t::print_info(const trame_in_t & in, trame_out_t & out)
{
  printf("Bar: init %i tpos %i %i pulse com %i pulse pid\terror %i "
	 "sum_integ %i pwm %i ir %i\n", 
	 in.bar_status_init, in.bar_pos_courante, in.bar_nb_pulse_com, in.bar_nb_pulse_pid,
	 in.bar_error,in.bar_sum_integ,in.bar_pwm, in.bar_ir_sensor);
  
}

