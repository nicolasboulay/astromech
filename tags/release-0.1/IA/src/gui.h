#ifndef GUI_H
#define GUI_H
#include <QApplication>
#include <QWidget>
#include <QVector>
#include <QThread>
#include <QTableWidget>
#include <string.h>

#include "task_rt.h"


class gui_t : public QWidget
{
  Q_OBJECT
  
    QTableWidget * tableWidget;
public:
    gui_t(task_rt_t * mt,QWidget *parent = 0);
public slots:
    void updateD(QVector<unsigned char> tab);
    void updateC(int i, int j,int val);

 private:
 int j;
};

#endif
