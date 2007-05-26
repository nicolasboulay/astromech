#ifndef COMPORTEMENT_TEST_H
#define COMPORTEMENT_TEST_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include "barillet.h"
#include "servo.h"

// class de test
class comportement_test_t : public comportement_t 
{
  barillet_t bar;

  int step;
  double starttime;
  servo_t servo;

  int step_bar;
  int step_mot_vit;
  
  double starttime_bar;
  double starttime_mot_vit;
  
  void init_test_barillet();
  bool test_barillet(const trame_in_t & tin,  trame_out_t & out,
		     internal_state_t & state);

  bool test_calibration_barillet(const trame_in_t & tin,  trame_out_t & out,
		     internal_state_t & state);

  void init_test_moteurs_vitesse();
  bool test_moteurs_vitesse(const trame_in_t & tin, trame_out_t & out, internal_state_t & state);

  int step_move;
  double starttime_move;
  void init_test_move();
  bool test_move(const trame_in_t & tin,  trame_out_t & out,
		     internal_state_t & state);
 public:


  comportement_test_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
		
};

#endif
