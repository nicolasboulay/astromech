#include "temporal_sequence.h"

// actionneur à sequencer :
//  -  servo_pelle ;
//  -  servo_ouverture;
//  -  servo_3; 


// should be initialised with state.current_time_s for example
void temporal_sequence_t::start_sequence(double _start_time){
  start_time=_start_time;
}


// les valeurs s'écrasent, cela évite un test à chaque fois.
bool temporal_sequence_t::testing(double current_time, trame_out_t & out)
{
  double delta = (current_time-start_time)*1000.0; // temps en ms
  
  if(delta < 1000.0) {
    out.servo_pelle    = 255;
    out.servo_ouverture= 255;
  }

  if(delta < 2000.0) {
    out.servo_pelle    = 254/2;
    out.servo_ouverture= 254/2;
  }

  if(delta < 3000.0) {
    out.servo_pelle    = 0;
    out.servo_ouverture= 0;
  }

  if(delta < 4000.0){
    return true;
  }

  return false;
}
