#ifndef COMPORTEMENT_DUMMY_H
#define COMPORTEMENT_DUMMY_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>


// class de test
class comportement_dummy_t : public comportement_t {
 public:
  comportement_dummy_t(int _name) : comportement_t(_name) {
    QTextStream qout(stdout);
      output.name("comportement_dummy");
      qout << "init comportement_dummy_t:" << _name << endl;
  }
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					  internal_state_t state) 
    {
      QTextStream qout(stdout);
      for(int n=0;n<REFEREE_NUMBER;n++){
	output.isActive[n]=false;
      }

      return output;
    };
};

#endif
