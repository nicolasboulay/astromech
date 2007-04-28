
#include <QApplication>
#include <QTableWidget>
#include <QString>
#include <QVBoxLayout>
#include "gui.h"
#include <QSize>


gui_t::gui_t(MyThread * mt,QWidget *parent)
  : QWidget(parent), j(0)
{
 
  tableWidget = new QTableWidget(12, 3, 0);
  tableWidget->setRowCount(32);
  tableWidget->setColumnCount(128);
  tableWidget->show();
  QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1"));
  tableWidget->setItem(0, 0, newItem);

  
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(tableWidget);
  setLayout(layout);

}

void gui_t::updateD(QVector<unsigned char> tab)
{

  printf("!!!!!!!!!!!!\n");
  //  tableWidget->setColumnCount(tab.count());
  //tableWidget->setRowCount(j+1);
  
  for(int i=0;i<tab.count();i++){
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(tab[i],2,16));
    tableWidget->setItem(j, i, newItem);
  }
  printf("j:%i\n",j);
  j++;
  tableWidget->resizeColumnsToContents();
  tableWidget->resizeRowsToContents();
}

void gui_t::updateC(int i, int j,int val)
{


  QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(val));
  tableWidget->setItem(j, i, newItem);
  //printf("j:%i\n",j);
  // update();
}
