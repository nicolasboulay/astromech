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
#include "trame_in.h"
#include "internal_state.h"

class gui_t : public QWidget
{
  Q_OBJECT
  
    QTableWidget * table_trame_in;
    QTableWidget * table_trame_out;

    //1er niveau arborescence
    QVector<QTreeWidgetItem*> tree_out;
    QVector<QTreeWidgetItem*> tree_in;
    QTreeWidgetItem* tree_state; 
    //2ième niveau
    QVector<QTreeWidgetItem*> tree_out_data; 
    QVector<QTreeWidgetItem*> tree_in_data; 
    QVector<QTreeWidgetItem*> tree_state_data; 

    void create_tree_out();
    void create_tree_in();
    void create_tree_state();
public:
    gui_t(task_rt_t * mt,QWidget *parent = 0);
public slots:
    void update_trame_out(QVector<unsigned char> tab);
    void update_trame_in(QVector<unsigned char> tab);
    void update_tree_out(const trame_out_t & out);
    void update_tree_in(const trame_in_t & in);
    void update_tree_state(const internal_state_t & state);
    void updateC(int i, int j,int val);

 private:
 int in_j;
 int out_j;
};

#endif
