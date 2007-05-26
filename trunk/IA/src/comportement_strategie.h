#ifndef COMPORTEMENT_STRATEGIE_H
#define COMPORTEMENT_STRATEGIE_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include "barillet.h"
#include "geom2D.h"
#include "servo.h"

#include <vector>

typedef vector <WayPoint *> vectorWaypoints;

// class de strategie
class comportement_strategie_t : public comportement_t 
{
  barillet_t barillet;
 public:
  unsigned int currentWaypointId;
  unsigned int nextWaypointId;
  bool objetOk;
  servo_t servo;
  
  vectorWaypoints listeWaypoints;
  
  comportement_strategie_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
};

#endif
