#include "barillet.h"

bool init(trame_in_t & in, trame_out_t & out)
{

  out.ctrl_bar_init=1;

  if(in.bar_status_init==0){
    return false;
  }
    return true;
}