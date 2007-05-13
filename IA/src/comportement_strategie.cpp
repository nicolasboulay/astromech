// Qt things
#include <QTextStream>
#include <QVector>

// std things
#include <cassert>
#include <complex>

//global IA things
#include "common.h"

// input/output/state of IA 
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"

// helper for manipulating mecanics of the robot
#include "barillet.h"


#include "comportement_strategie.h"


 comportement_strategie_t::comportement_strategie_t(int _name) : comportement_t(_name) 
{
    QTextStream qout(stdout);
    output.name("strategie comportement");
    qout << "init comportement_strategie:" << _name << endl;
}

 comportement_result_t 
comportement_strategie_t::execute(trame_in_t tin,  
			     internal_state_t & state) 
{
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=true;
  }


  return output;
};
