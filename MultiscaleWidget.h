#pragma once

#include "globals.h"
#include "node.h"

#include <QMainWindow>
#include <QApplication>
#include <QColor>
#include <QLineEdit>
#include <QStatusBar>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>

class MultiscaleWidget : public QMainWindow
{
  Q_OBJECT

  public:
    MultiscaleWidget(QWidget *parent = 0);

  protected:
    void paintEvent(QPaintEvent *event);
    void drawLines(QPainter *qp);

  private:
    QColor *colors;
    Node **nodes;
    Node **next;
    QStatusBar *status;
    QComboBox *cb;
    bool redraws;
    bool energyMode;
    QLineEdit *ene_le;
    QLineEdit *steps_le;
    QPushButton *ene_bt;
    QProgressBar *pb;
    QComboBox *nuc_mode;
    QLineEdit *nuc_every;
    QLineEdit *nuc_change;
    int nucs_step;

    void prepareNodes();
    void generateColors();
    void clearNext();
    void copyNextToCurrent();


  private slots:
    void PrepareNodesSlot();
    void MCStepSlot();
    void DistributeEnergySlot();
    void ShowEnergyMapSlot();
    void NucleationSlot();
 
};