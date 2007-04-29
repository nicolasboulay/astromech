/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QApplication>
//#include <QPushButton>
#include <QTableWidget>
#include <QString>
#include <QThread>
#include "gui.h"
#include "serial.h"
#include <iostream>
#include "trame_out.h"
#include <QVector>
#include <QtGui>
#include <QObject>
using namespace std;

// class MyThread : public QThread
// {    
//   Q_OBJECT
// public:
//   void run();
//   MyThread(char* _device) {device=_device;}
// private:
//   char * device;

//   signals:
//   void newTrame(QVector<unsigned char> tab);
// };

void MyThread::run()
{

    serial_t com(device);
    trame_binary_t tbin;
    trame_binary_t tbout;
    trame_out_t out;

    out.led1_jaune = 1;
    out.led1_vert = 0;
    out.led1_orange = 1;
    out.led2_rouge=0;
    out.led2_orange=1;
    out.led2_jaune=1;
    out.led3_vert=1;
    out.led3_jaune=1;
    out.led3_orange=0;
    out.led3_rouge=0;
    int j=0;
  while(1){
    
    cout << "---------------------------------------------------\n";
    //    tbout[12]=i++;
    out.led1_jaune  ^= 1;
    out.led1_vert   ^= 1;
    out.led1_orange ^= 1;
    out.led2_rouge  ^= 1;
    out.led2_orange ^= 1;
    out.led2_jaune  ^= 1;
    out.led3_vert   ^= 1;
    out.led3_jaune  ^= 1;
    out.led3_orange ^= 1;
    out.led3_rouge  ^= 1;


    out.serialise(tbout);
    tbout.inc_num_trame();
    tbout.gen_valid_pc_pic_paquet();
    tbout.gen_valid_trame();
    emit newTrame(tbout);
    tbout.dump_on_file("trame_from_pc.txt");
//     for (int i=0;i<tbout.count();i++){
//       emit newTrameC(i,j,tbout[i]);
//     }
//     j=(j+1)%32;
    cout << "out:";
    tbout.print();
    cout << endl;
    cout << "valide trame ?" << tbout.validation_trame() << endl;
    cout << "valide pic ?" << tbout.validation_paquet_pc_pic1() << endl;


    com.exchange(tbin,tbout);
    cout << "in:";
    tbin.print_from_pic();
    cout << endl;
    cout << "valide trame ?" << tbin.validation_trame() << endl;
    cout << "validation ?" << tbin.validation() << endl;
    tbin.dump_on_file("trame_from_pic.txt");

    sleep(1);
  }
  
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  
  MyThread mt(argv[1]);


  gui_t gui(&mt);
  qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");
   QObject::connect(&mt, SIGNAL(newTrame(QVector<unsigned char>)),
 		   &gui, SLOT(updateD(QVector<unsigned char>)));//,Qt::DirectConnection);
//   QObject::connect(&mt, SIGNAL(newTrameC(int, int, int)),
// 		   &gui, SLOT(updateC(int, int, int)));//,Qt::DirectConnection);

  mt.dumpObjectInfo ();
  gui.dumpObjectInfo ();
  mt.start();

  gui.show();

  return app.exec();
}


