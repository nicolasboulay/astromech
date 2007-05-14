#include "comportement_async.h"

comportement_result_t  
comportement_async_t::execute(const trame_in_t & tin, 
			      internal_state_t & state) 
{
  if(first_time){
    start(QThread::NormalPriority);
    first_time=false;
  }

  mutex.lock();
  tin_ext=tin;
  state_ext=state;
  output=result_ext; //une recopie de plus mais bon...
  mutex.unlock();

  return output;
}
