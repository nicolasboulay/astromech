#include"comportement_eye.h"
#include <QThread>
#include <QTextStream>
#include "eye_processing.h"

comportement_eye_t::comportement_eye_t(int _name) : 
  comportement_async_t(_name)
{
    QTextStream qout(stdout);
    output.name("eye comportement");
    qout << "init comportement_eye:" << _name << endl;

}

void comportement_eye_t::run(){
  comportement_result_t result_intra;
  trame_in_t tin_intra;
  internal_state_t state_intra;
  eye_processing_t eye;
  
  eye.init_device();
  
  forever {
    mutex.lock();
    trame_in_t tin_intra =tin_ext;
    internal_state_t state_intra = state_ext;
    mutex.unlock();
    
    eye.grab_frame();
    eye.panier_processing();

    mutex.lock();
    result_ext=result_intra;
    mutex.unlock();
  }
}

