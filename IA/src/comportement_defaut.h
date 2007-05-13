#ifndef COMPORTEMENT_DEFAUT_H
#define COMPORTEMENT_DEFAUT_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>


// class de test
class comportement_defaut_t : public comportement_t {
 public:
  comportement_defaut_t(int _name) : comportement_t(_name) {
    QTextStream qout(stdout);
    output.name("default comportement");
    qout << "init comportement_default:" << _name << endl;
  }
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					  internal_state_t & state) 
    {
      for(int n=0;n<REFEREE_NUMBER;n++){
	output.isActive[n]=true;
      }
      return output;
    };
};

#endif
