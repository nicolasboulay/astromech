#ifndef GUI_H
#define GUI_H
#include <QApplication>
#include <QWidget>
#include <QVector>
#include <QThread>
#include <QTableWidget>
#include <string.h>
#include <QTreeWidgetItem>
#include "task_rt.h"
#include "trame_out.h"


class gui_t : public QWidget
{
  Q_OBJECT
  
    //QTableWidget * tableWidget;
    QTableWidget * table_trame_in;
    QTableWidget * table_trame_out;
    QVector<QTreeWidgetItem*> tree_out;
    QTreeWidgetItem *tree_in;
    QVector<QTreeWidgetItem*> tree_out_data; 
    void create_tree_out();
    //    template <typename T_t> qvector_to_qstring(QVector<T_t> x);
public:
    gui_t(task_rt_t * mt,QWidget *parent = 0);
public slots:
    void update_trame_out(QVector<unsigned char> tab);
    void update_trame_in(QVector<unsigned char> tab);
    void update_tree_out(const trame_out_t & out);
    void updateC(int i, int j,int val);

 private:
 int in_j;
 int out_j;
};

#endif
