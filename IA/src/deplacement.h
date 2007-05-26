#ifndef DEPLACEMENT_H
#define DEPLACEMENT_H
#include "trame_in.h"
#include "trame_out.h"
#include "common.h"
#include "geom2D.h"

#define WP_FORCE 0
#define WP_WAIT 1

#define EN_AVANT 1
#define EN_ARRIERE 0

#define ROTATION 1
#define DEPLACEMENT 0

#define HORAIRE 1
#define ANTIHORAIRE 0

class deplacement_t {
 public:
  bool getWaypointCibleId ( int numero_01, unsigned &id, const trame_in_t & in );
  bool setWaypoint (int numero_01, unsigned id, WayPoint *wp, int wp_next, int marche, int deplacement, int rotation, trame_out_t & out );
  bool setWaypoint (int numero_01, unsigned id, double x, double y, double cap_deg, double vitesse_mps, int wp_next, int marche, int deplacement, int rotation, trame_out_t & out );
  bool initAsserv (trame_out_t & out);
  bool print_info(const trame_in_t & in);
  bool mode_pwm(int speed_r,int speed_l,trame_out_t & out);
  bool mode_vitesse(int speed_r,int speed_l,trame_out_t & out);
  bool mode_wp(trame_out_t & out);
  bool initNav(int pos_x, int pos_y, int cap, trame_out_t & output );
  bool finInitNav(trame_out_t & output);
  
 private:
  
};

#endif
