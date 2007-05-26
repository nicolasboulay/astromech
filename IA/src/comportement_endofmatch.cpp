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
#include <QTime>


// inline qulonglong GetTick()
// {

//   qulonglong x;
//   /* __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));*/
//   __asm__ volatile ("RDTSC" : "=A" (x));
//   return x;
// }

double get_current_time_s()
{
  QTime t;
  t.start();
  return 
    t.hour()*3600+
    t.minute()*60+
    t.second()+
    t.msec()/1000.0;
}

comportement_endofmatch_t::comportement_endofmatch_t(int _name) : 
  comportement_t(_name), first_execute(true)
{
    QTextStream qout(stdout);
    output.name("endofmatch comportement");
    qout << "init comportement_endofmatch:" << _name << endl;
}

// double tick_convert_to_second(qulonglong time, qulonglong onesecond)
// {
//   double ret;

//   ret=(time*1.0)/onesecond;
//   return ret;
// }

comportement_result_t 
comportement_endofmatch_t::execute(const trame_in_t & tin,  
			     internal_state_t & state) 
{ 
  QTextStream qout(stdout);
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=false;
  }

  state.current_time_s = get_current_time_s();

  if(state.match_ongoing){
    if (tin.start ==0){
      // remise à zéro par insertion du jack de départ
      state.match_ongoing=false;
    } else {
      // en match, verification du chrono
      double _elasped_time_s = state.current_time_s-state.start_time;
      printf("Match start since : t=%f\n",_elasped_time_s);
      state.elasped_time_s=_elasped_time_s;
      if (92< _elasped_time_s) 
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
      state.start_time=get_current_time_s();
    }
    //sinon ne rien faire du tout (cas debug)
  }

  //ouput est une version neutre
  return output;
};
