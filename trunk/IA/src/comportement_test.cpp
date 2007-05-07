#include "comportement.h"
#include <QTextStream>
#include <assert.h>
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement_test.h"
#include <QVector>
#include "common.h"
#include <complex>

 comportement_test_t::comportement_test_t(int _name) : comportement_t(_name) 
{
    QTextStream qout(stdout);
    output.name("test comportement");
    qout << "init comportement_test:" << _name << endl;
}

 comportement_result_t 
comportement_test_t::execute(trame_in_t tin,  
			     internal_state_t & state) 
{
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=true;
  }

  complex<double> c;

  return output;
};
