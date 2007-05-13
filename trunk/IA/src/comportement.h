#ifndef COMPORTEMENT_H
#define COMPORTEMENT_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include <QVector>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <iostream>
using namespace std;
class comportement_result_t : public trame_out_t 
{  
  public:
  comportement_result_t() : trame_out_t(), isActive(100,false) {}
  QString name(){return _name;}
  QString name(QString n){_name=n;return _name;}
  
  QVector<bool> isActive;
  QString _name;   //comportement name used for debugging purpose
};

class comportement_t 
{
    public :
    virtual ~comportement_t() {}
    comportement_t(int _name=-1) : name(_name), output() { }
    virtual comportement_result_t  execute(const trame_in_t & tin, 
					    internal_state_t & state) {
      cout << "comportement_t !\n";
      return output;
    };
 protected:
    int name; //comportement "name" used for some indexing
  comportement_result_t output;
};

#endif
