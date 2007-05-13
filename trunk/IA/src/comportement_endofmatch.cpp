#include "comportement.h"
#include <QTextStream>
#include <assert.h>
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement_endofmatch.h"
#include <QVector>
#include "common.h"
#include <complex>


inline qulonglong GetTick()
{

  qulonglong x;
  /* __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));*/
  __asm__ volatile ("RDTSC" : "=A" (x));
  return x;
}

comportement_endofmatch_t::comportement_endofmatch_t(int _name) : 
  comportement_t(_name), first_execute(true)
{
    QTextStream qout(stdout);
    output.name("endofmatch comportement");
    qout << "init comportement_endofmatch:" << _name << endl;
}

double tick_convert_to_second(qulonglong time, qulonglong onesecond)
{
  double ret;

  ret=(time*1.0)/onesecond;
  return ret;
}

comportement_result_t 
comportement_endofmatch_t::execute(const trame_in_t & tin,  
			     internal_state_t & state) 
{ 
  QTextStream qout(stdout);
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=false;
  }
  if(first_execute){
    //ne pas pas aller dans le constructeur à cause de state
    first_execute=false;
    qulonglong start = GetTick();
    sleep(1);
    state.onesecond_in_tick = GetTick()-start;
    qout << "  one second is " << state.onesecond_in_tick << " ticks\n"; 
  }
  state.current_time_s = (1.0*GetTick())/state.onesecond_in_tick;

  if(state.match_ongoing){
    if (tin.start ==0){
      // remise à zéro par insertion du jack de départ
      state.match_ongoing=false;
    } else {
      // en match, verification du chrono
      double elasped_time_s = tick_convert_to_second(GetTick()-state.start_time,
						     state.onesecond_in_tick);
      printf("t=%f\n",elasped_time_s);
      state.elasped_time_s=elasped_time_s;
      if (92< elasped_time_s) 
	{
	  // si la minute 30 est passé ont stop tout !
	  output.isActive[REFEREE_DEPLACEMENT]=true;
	  output.isActive[REFEREE_TOOLS]=true;
	}
    }
  } else {
    if (tin.start !=0){
      // jack retiré : début du match !
      state.match_ongoing=true;
      state.start_time=GetTick();
    }
    //sinon ne rien faire du tout (cas debug)
  }

  //ouput est une version neutre
  return output;
};
