#ifndef GUI_H
#define GUI_H
#include <QApplication>
#include <QWidget>
#include <QVector>
#include <QThread>
#include <QTableWidget>
class MyThread : public QThread
{    
  Q_OBJECT

public:
  void run();
  MyThread(char* _device) {device=_device;}

signals:
  void newTrame(QVector<unsigned char> tab);
  void newTrameC(int i, int j,int val);
private:
  char * device;
  int j;

};

class gui_t : public QWidget
{
  Q_OBJECT
  
    QTableWidget * tableWidget;
public:
    gui_t(MyThread * mt,QWidget *parent = 0);
public slots:
    void updateD(QVector<unsigned char> tab);
    void updateC(int i, int j,int val);

 private:
 int j;
};

#endif