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
    
    //DEBUG !
    // à faire sur appuis du bon bouton !
    // if(tin_intra.b...)
    //eye.take_before_match_picture();

    // if(state_intra.match_ongoing){
      // debut de match avant la 1er secondes
      //if(state_intra.elasped_time_s < 1.0){

	// QVector<float> proba_panier(3,0);
// 	eye.panier_processing(proba_panier);
// 	state_intra.panier.proba_presence[0]=proba_panier[0];
// 	state_intra.panier.proba_presence[1]=proba_panier[1];
// 	state_intra.panier.proba_presence[2]=proba_panier[2];
	//}
	//}

    eye.target_processing();

    mutex.lock();
    state_ext = state_intra; /// DEBUG y'a un soucis pour retransmètre le internal_state;
    result_ext=result_intra;
    mutex.unlock();
  }
}

