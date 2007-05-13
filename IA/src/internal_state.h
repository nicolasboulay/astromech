#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H
#include <QtGlobal>
#include "robot_adverse.h"
#include "panier.h"
// this is the repository of persistent data inside the robot

class internal_state_t {
 public:
  // Navigation interne
  double x;
  double y;
  double cap; // 0 à 360°

  // match démarer
  bool match_ongoing;
  bool we_are_blue;
  bool we_are_red;

  //gestion du temps
  qulonglong start_time;
  qulonglong onesecond_in_tick;
  double elasped_time_s;
  double current_time_s;

  //comportement élue précédent
  int previous_comportement;

  //current waypoint
  complex<double> wp_position;
  double wp_cap;

  //obstacles
  panier_t panier;
  robot_adverse_t robot_adverse;

  internal_state_t() :  
    x(-1.0),y(-1.0),cap(-1.0),
    match_ongoing(false),
    we_are_blue(false), we_are_red(false),
    start_time(0), onesecond_in_tick(0),elasped_time_s(0), current_time_s(0)

    {}
};

#endif
