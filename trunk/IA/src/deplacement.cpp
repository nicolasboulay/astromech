#include "deplacement.h"
#include "geom2D.h"
#include <cmath>

// Lit le waypoint en cours ou le prochain waypoint sur le PIC.
bool deplacement_t::getWaypointCibleId ( int numero_01, unsigned &id, const trame_in_t & in ) {

	id = in.current_wp_number;
	return true;
	
}


bool deplacement_t::setWaypoint (int numero_01, unsigned id, WayPoint *wp, int wp_next, int marche, int deplacement, int rotation, trame_out_t & out ) {
	return setWaypoint(numero_01, id, wp->pt->x, wp->pt->y, wp->cap_deg, wp->vitesse_m_par_s, wp_next, marche, deplacement, rotation, out);
}

// Exporte un waypoint vers la trame de sortie, le cap_x et cap_y est calculé à la volée.
bool deplacement_t::setWaypoint ( int numero_01, unsigned id, double x,
			    double y, double cap_deg, double vitesse_mps, int wp_next, int marche, int deplacement, int rotation, trame_out_t & out ) { 
	
	double cap_x = 1000 * cos(convertirAngle(deg2rad(cap_deg)));
	double cap_y = 1000 * sin(convertirAngle(deg2rad(cap_deg)));

/*

  QVector<int> waypoint_ctrl_wp_nul;
  QVector<int> waypoint_ctrl_wp_next;
  QVector<int> waypoint_ctrl_sens_wp;
  QVector<int> waypoint_ctrl_rot;
  QVector<int> waypoint_ctrl_sens_rot;
  QVector<int> waypoint_ctrl_pwm;

*/
	if ( numero_01 == 0 || numero_01 == 1 ) {
		
		int i = numero_01;
		out.waypoint_number[i] = (uint8_t) id;
		out.waypoint_pos_x[i]  = (x * 1000);
		out.waypoint_pos_y[i]  = (y * 1000);
		out.waypoint_cap[i]    = cap_deg;
		out.waypoint_cap_x[i]  = cap_x;
		out.waypoint_cap_y[i]  = cap_y;
		out.waypoint_speed[i]  = vitesse_mps * 100;
		out.waypoint_ctrl_wp_next[i] = wp_next; // forçage ou pas
		out.waypoint_ctrl_sens_wp[i] = marche; // avant/arrière
		out.waypoint_ctrl_rot[i] = deplacement; // rotation
		out.waypoint_ctrl_sens_rot[i] = rotation;
		out.waypoint_ctrl_pwm[i] = 0;
		printf("Cas %d OK\n", numero_01);
		return true;
	
	} else {
		return false;
	}
}

bool deplacement_t::initAsserv ( trame_out_t & out) 
{
  // Initialisation des gains des moteurs de propulsion
  
  out.left_gain_prop = 8;
  out.left_gain_integ = 1;
  out.left_gain_deriv = 0;
  out.left_satur_sum_integ = 400;
  out.left_thres_prop_only = 30;
  
  out.right_gain_prop = 8;
  out.right_gain_integ = 1;
  out.right_gain_deriv = 0;
  out.right_satur_sum_integ = 400;
  out.right_thres_prop_only = 30;
	
	return true;
			
}

bool deplacement_t::print_info(const trame_in_t & in)
{
  printf("MOVE: x %i y %i cap %i speed %i   wp %i->%i "
	 "pulse %i|%i %i|%i  error %i|%i sum_integ %i|%i "
	 "pwm %i|%i\n",
	 in.x,in.y,in.cap,in.speed, 
	 in.current_wp_number, in.next_wp_number, 
	 in.left_nb_pulse_com, in.right_nb_pulse_com,
	 in.left_nb_pulse_pid, in.right_nb_pulse_pid,
	 in.left_error,  in.right_error,
	 in.left_sum_integ,  in.right_sum_integ,
	 in.left_pwm_motor,  in.right_pwm_motor);
  return true;
}

bool deplacement_t::mode_pwm(int speed_r,int speed_l,trame_out_t & out)
{
  out.mode_pwm    =1;
  out.mode_vitesse=0;
  out.mode_wp     =0;
  out.right_pwm_motor=speed_r;
  out.left_pwm_motor =speed_l;
  return true;
}


bool deplacement_t::mode_vitesse(int speed_r,int speed_l,trame_out_t & out)
{
  out.mode_pwm    =0;
  out.mode_vitesse=1;
  out.mode_wp     =0;
  out.consigne_speed_right=speed_r;
  out.consigne_speed_left =speed_l;
  return true;
}

bool deplacement_t::mode_wp(trame_out_t & out)
{
	out.mode_pwm           = 0;
	out.mode_vitesse       = 0;
	out.mode_wp            = 1;
	return true;
}

bool deplacement_t::initNav(int pos_x, int pos_y, int cap, trame_out_t & output ) 
{
	output.nav_ctrl_update = 1;
	output.new_position_x = pos_x * 1000;
	output.new_position_y = pos_y * 1000;
	output.new_cap = cap;
	output.quat_w =  10000 * cos((cap*M_PI/180)/2);
	output.quat_z = -10000 * sin((cap*M_PI/180)/2);
	return true;
	
}

bool deplacement_t::finInitNav(trame_out_t & output) {
	output.nav_ctrl_update = 0;
	return true;	
}

