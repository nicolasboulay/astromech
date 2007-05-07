#include <iostream>
#include "manager.h"
#include "trame_out.h"
#include "trame_in.h"
#include "serial.h"
#include "task_rt.h"

using namespace std;

 void task_rt_t::run2()
 {

//     serial_t com(device);
//     trame_binary_t tbin;
//     trame_binary_t tbout;
//     trame_out_t out;
//     trame_in_t in;

//     out.led1_jaune = 1;
//     out.led1_vert = 0;
//     out.led1_orange = 1;
//     out.led2_rouge=0;
//     out.led2_orange=1;
//     out.led2_jaune=1;
//     out.led3_vert=1;
//     out.led3_jaune=1;
//     out.led3_orange=0;
//     out.led3_rouge=0;
//         int j=0;

//     manager_t manager;

//     out=manager.execute(in);
//     //    out.print();

//   while(1){
    
//     cout << "---------------------------------------------------\n";
//     //    tbout[12]=i++;
//     out.led1_jaune  ^= 1;
//     out.led1_vert   ^= 1;
//     out.led1_orange ^= 1;
//     out.led2_rouge  ^= 1;
//     out.led2_orange ^= 1;
//     out.led2_jaune  ^= 1;
//     out.led3_vert   ^= 1;
//     out.led3_jaune  ^= 1;
//     out.led3_orange ^= 1;
//     out.led3_rouge  ^= 1;


//     out.serialise(tbout);
//     tbout.inc_num_trame();
//     tbout.gen_valid_pc_pic_paquet();
//     tbout.gen_valid_trame();
//     emit newTrameOut(tbout);
//     tbout.dump_on_file("trame_from_pc.txt");

//     cout << "out:";
//     tbout.print();
//     cout << endl;
//     cout << "valide trame ?" << tbout.validation_trame() << endl;
//     cout << "valide pic ?" << tbout.validation_paquet_pc_pic1() << endl;


//     com.exchange(tbin,tbout);
//     emit newTrameIn(tbin);
//     cout << "in:";
//     tbin.print_from_pic();
//     cout << endl;
//     cout << "valide trame ?" << tbin.validation_trame() << endl;
//     cout << "validation ?" << tbin.validation() << endl;
//     tbin.dump_on_file("trame_from_pic.txt");

//     sleep(1);
//   }
  
 }

void task_rt_t::run()
{
  serial_t comRS232(device);
  trame_binary_t tbin;
  trame_binary_t tbout;
  trame_out_t out;
  trame_in_t in;    
  manager_t manager;
  internal_state_t state;

  emit newTreeIn(in); // for debugging without the robot...

  forever{
    out=manager.execute(in,state); 
    emit newTreeState(state);
    out.serialise(tbout);
    tbout.gen_valid_header_trailer();
    tbout.dump_on_file("trame_from_pc.txt");
    emit newTrameOut(tbout);
    emit newTreeOut(out);
    comRS232.exchange(tbin,tbout);
    emit newTrameIn(tbin);
    if(tbin.validation()){
      // deserialisation uniquement si la trame est valide.
      // sinon on conserve la donnée précédente
      // TODO:Il faudrait mettre qq choses dans l'état du robot
      in.deserialise(tbin);
      emit newTreeIn(in);
    }
    tbin.dump_on_file("trame_from_pic.txt");
    sleep(1);
  }
}
