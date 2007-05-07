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
#include "gui.h"
#include <iostream>
#include <QtGui>
#include "CImg.h"

#include "task_rt.h"

using namespace std;

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


  task_rt_t mt(dev);


  if(use_gui){
    gui_t gui(&mt);
    cout << "Launch gui\n";
    qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");
    qRegisterMetaType<trame_out_t>("trame_out_t");
    qRegisterMetaType<trame_in_t>("trame_in_t");
    qRegisterMetaType<internal_state_t>("internal_state_t");
    QObject::connect(&mt, SIGNAL(newTrameOut(QVector<unsigned char>)),
		     &gui, SLOT(update_trame_out(QVector<unsigned char>)));//,Qt::DirectConnection);
    QObject::connect(&mt, SIGNAL(newTrameIn(QVector<unsigned char>)),
		     &gui, SLOT(update_trame_in(QVector<unsigned char>)));//,Qt::DirectConnection);
    QObject::connect(&mt, SIGNAL(newTreeOut(const trame_out_t &)),
		     &gui, SLOT(update_tree_out(const trame_out_t &)));//,Qt::DirectConnection);
    QObject::connect(&mt, SIGNAL(newTreeIn(const trame_in_t &)),
		     &gui, SLOT(update_tree_in(const trame_in_t &)));//,Qt::DirectConnection);
    QObject::connect(&mt, SIGNAL(newTreeState(const internal_state_t &)),
		     &gui, SLOT(update_tree_state(const internal_state_t &)));//,Qt::DirectConnection);

    mt.start();
    //gui.show();
    return app.exec();
  } else {
    mt.start();
    return app.exec();
  }

  //mt.dumpObjectInfo ();
  //gui.dumpObjectInfo ();

  return app.exec();
}


