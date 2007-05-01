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
#include "CImg.h"

#include "manager.h"

using namespace std;

void MyThread::run()
{

    serial_t com(device);
    trame_binary_t tbin;
    trame_binary_t tbout;
    trame_out_t out;
    trame_in_t in;

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

    manager_t manager;

    out=manager.execute(in);
    //    out.print();

    exit();
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

  // Display program usage, when invoked from the command line with option '-h'.
  cimg_usage("Astromech IA : ./ia [-dev /dev/ttyS0] [-nogui]");


  // Read device filename from the command line (or set to /dev/ttyS0)"

  const char * dev
    = cimg_option("-dev","/dev/ttyS0","RS232 device");
  bool use_gui
    = !(cimg_option("-nogui",false,"Starting without gui"));


  MyThread mt(dev);

  
  if(use_gui){
    gui_t gui(&mt);
    qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");
    QObject::connect(&mt, SIGNAL(newTrame(QVector<unsigned char>)),
		     &gui, SLOT(updateD(QVector<unsigned char>)));//,Qt::DirectConnection);
    mt.start();
    gui.show();
  } else {
    mt.start();
  }

  //mt.dumpObjectInfo ();
  //gui.dumpObjectInfo ();

  return app.exec();
}


