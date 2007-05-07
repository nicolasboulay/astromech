#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H

// this is the repository of persistent data inside the robot

class internal_state_t {
 public:
  double x;
  double y;
  double cap; // 0 à 360°
  bool match_ongoing;

  qulonglong start_time;
  qulonglong onesecond_in_tick;
  double elasped_time_s;

  internal_state_t() : 
    match_ongoing(false),start_time(0), onesecond_in_tick(0),elasped_time_s(0),
    x(-1.0),y(-1.0),cap(-1.0)
    {}
};

#endif
