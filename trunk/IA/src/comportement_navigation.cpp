#include"comportement_navigation.h"
#include <QThread>
#include <QTextStream>

comportement_navigation_t::comportement_navigation_t(int _name) : 
  comportement_t(_name)
{
    QTextStream qout(stdout);
    output.name("navigation comportement");
    qout << "init comportement_navigation:" << _name << endl;

    //DEBUG !
    // traitement_cone.start(QThread::NormalPriority);
}

comportement_result_t  comportement_navigation_t::execute(const trame_in_t & tin, 
				       internal_state_t & state)
{
  // run in the task_rt thread :
  // - Changed the internal_state for the cap and position
  // - play() stop() depending on internal_state command
  // - navigation inertiel ?
  return output;
}
