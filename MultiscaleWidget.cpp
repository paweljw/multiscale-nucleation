#include "MultiscaleWidget.h"
#include "MonteCarloProcessor.h"
#include "NucleationProcessor.h"

#include <QApplication>
#include <QPainter>
#include <QLabel>
#include <QFrame>

#include <iostream>

// nucleation:
// - on/off
// - every
// - mode (const/inc/dec)
// - change

// perform X steps of nucleation

MultiscaleWidget::MultiscaleWidget(QWidget *parent)
    : QMainWindow(parent)
{
  energyMode = false;
  redraws = false;

  //---------- quitting button
  QPushButton *quit_bt = new QPushButton("Quit", this);
  quit_bt->setGeometry(WIDTH + 20, 10, 170, 30);
  connect(quit_bt, SIGNAL(clicked()), qApp, SLOT(quit()));

  steps_le = new QLineEdit("10", this);
  steps_le->setGeometry(WIDTH + 20, 70, 170, 30);

  //---------- MC button
  QPushButton *mc_bt = new QPushButton("Produce microstructure", this);
  mc_bt->setGeometry(WIDTH + 20, 100, 170, 30);
  connect(mc_bt, SIGNAL(clicked()), this, SLOT(MCStepSlot()));

  //---------- Reset button
  QPushButton *reset_bt = new QPushButton("Reset field", this);
  reset_bt->setGeometry(WIDTH + 20, 40, 170, 30);
  connect(reset_bt, SIGNAL(clicked()), this, SLOT(PrepareNodesSlot()));

  QFrame* line = new QFrame(this);
  line->setObjectName(QString::fromUtf8("line"));
  line->setGeometry(QRect(WIDTH+20, 136, 170, 3));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  //---------- Reset button
  cb = new QComboBox(this);
  cb->insertItem(0, "Homogenous");
  cb->insertItem(1, "Heterogenous");
  cb->setGeometry(WIDTH + 20, 160, 170, 30);

  QLabel *lab = new QLabel("Energy distribution:", this);
  lab->setGeometry(WIDTH + 22, 135, 170, 30);

  QPushButton *dist_bt = new QPushButton("Distribute energy", this);
  dist_bt->setGeometry(WIDTH + 20, 190, 170, 30);
  connect(dist_bt, SIGNAL(clicked()), this, SLOT(DistributeEnergySlot()));

  ene_bt = new QPushButton("Show energy map", this);
  ene_bt->setGeometry(WIDTH + 20, 220, 170, 30);
  connect(ene_bt, SIGNAL(clicked()), this, SLOT(ShowEnergyMapSlot()));  

  ene_le = new QLineEdit("1.000", this);
  ene_le->setGeometry(WIDTH + 20, 260, 170, 30);

  QFrame* line2 = new QFrame(this);
  line2->setObjectName(QString::fromUtf8("line"));
  line2->setGeometry(QRect(WIDTH+20, 254, 170, 3));
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
 
  pb = new QProgressBar(this);
  pb->setGeometry(WIDTH + 20, 780, 170, 30);

  nuc_mode = new QComboBox(this);
  nuc_mode->insertItem(0, "Constant");
  nuc_mode->insertItem(1, "Increasing");
  nuc_mode->insertItem(2, "Decreasing");
  nuc_mode->setGeometry(WIDTH+20, 290, 170, 30);

  QLabel *nucl = new QLabel("Nucleation every:", this);
  nucl->setGeometry(WIDTH+23, 315, 170, 30);

  nuc_every = new QLineEdit("10", this);
  nuc_every->setGeometry(WIDTH+20, 337, 170, 30);

  QLabel *nucl2 = new QLabel("Nucleation change:", this);
  nucl2->setGeometry(WIDTH+23, 363, 170, 30);

  nuc_change = new QLineEdit("1", this);
  nuc_change->setGeometry(WIDTH+20, 383, 170, 30);

  QPushButton *nucleate = new QPushButton("Run nucleations", this);
  nucleate->setGeometry(WIDTH+20, 413, 170, 30);
  connect(nucleate, SIGNAL(clicked()), this, SLOT(NucleationSlot()));

  generateColors();
  prepareNodes();
  clearNext();
  redraws = true;
}

void MultiscaleWidget::NucleationSlot()
{
  nucs_step = NUCLEATIONS;

  pb->setMinimum(0);
  pb->setMaximum(steps_le->text().toInt());
  pb->setValue(0);

  for(int i=0; i< steps_le->text().toInt(); i++)
  {
    clearNext();

    if(!(i%nuc_every->text().toInt()))
    {
      // apply nucleation number change (if applicable)
      if (nuc_mode->currentIndex() == 1)
      {
        nucs_step += nuc_change->text().toInt();
      } else if(nuc_mode->currentIndex() == 2) {
        nucs_step -= nuc_change->text().toInt();      
      }
      NucleationProcessor::SpreadNucleations(nodes, nucs_step, cb->currentIndex()); // apply actual nucleations
    }

    NucleationProcessor::processField(nodes, next, ene_le->text().toFloat());

    copyNextToCurrent();
    pb->setValue(i+1);
    this->repaint();
  }

  
  this->repaint();
}

void MultiscaleWidget::ShowEnergyMapSlot()
{
  if (energyMode)
    energyMode = false;
  else
    energyMode = true;

  this->repaint();
}

void MultiscaleWidget::DistributeEnergySlot()
{
  redraws = false; 

  QString tt = ene_le->text();

  if (cb->currentIndex() == 0) // homogenous
  {
    for(int i=0;i<DIM; i++)
      for(int j=0; j<DIM; j++)
      {
        nodes[i][j].energy = ENERGY_MAX / 2.0f;
      }
  } else {
    for(int i=0;i<DIM; i++)
      for(int j=0; j<DIM; j++)
      {
        if(MonteCarloProcessor::IsGrainBoundary(nodes, i, j))
          nodes[i][j].energy = ENERGY_MAX;
        else
          nodes[i][j].energy = 0.0f;
      }
  }

  redraws = true;
  this->repaint();
}

void MultiscaleWidget::MCStepSlot()
{
//  redraws = false;

  pb->setMinimum(0);
  pb->setMaximum(steps_le->text().toInt());
  pb->setValue(0);

  for(int i=0; i< steps_le->text().toInt(); i++)
  {
    clearNext();
    MonteCarloProcessor::processField(nodes, next);
    copyNextToCurrent();
    pb->setValue(i+1);
    this->repaint();
  }
//  redraws = true;
  
}

void MultiscaleWidget::prepareNodes()
{
  redraws = false;
  //---------- prepare a MC play field
  nodes = new Node*[DIM];
  for(int i=0; i<DIM; i++)
  {
    nodes[i] = new Node[DIM];
    for(int j=0; j<DIM; j++)
    {
      nodes[i][j].id = i*DIM+j + 1;
      nodes[i][j].energy = 0.0f;
      nodes[i][j].recryst = false;
    }
  }

  next = new Node*[DIM];
  for(int i=0; i<DIM; i++)
  {
    next[i] = new Node[DIM];
  }

  redraws = true;
  this->repaint();
}

void MultiscaleWidget::copyNextToCurrent()
{
  for(int i=0; i<DIM; i++)
    for(int j=0; j<DIM; j++)
      nodes[i][j] = next[i][j];
}

void MultiscaleWidget::generateColors()
{
  colors = new QColor[2*(DIM*DIM+1)];

  colors[0] = Qt::black;
  for(int i=1; i<DIM*DIM+1; i++)
  {
    int g = 40 + static_cast<int> ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/215.0f)) );
    int b = 40 + static_cast<int> ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/215.0f)) );
    colors[i] = QColor::fromRgb(0, g, b);
  }

  for(int i=DIM*DIM+1; i<2*(DIM*DIM+1); i++)
  {
    int r = 40 + static_cast<int> ( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/175.0f)) );
    colors[i] = QColor::fromRgb(r, 0, 0);
  }  

}

void MultiscaleWidget::PrepareNodesSlot()
{
  prepareNodes();
}

void MultiscaleWidget::paintEvent(QPaintEvent *e)
{
  Q_UNUSED(e);
  QPainter qp(this);
  drawLines(&qp);
}

void MultiscaleWidget::drawLines(QPainter *qp)
{  
  if(redraws)
  {
    qp->setPen(Qt::NoPen);

    for(int i = 0; i < DIM; i++)
      for(int j = 0; j < DIM; j++)
      {
        if (energyMode)
        {
          float ene = nodes[i][j].energy / ENERGY_MAX;
          int col = (int) (ene * 255.0f);
          qp->setBrush(QBrush(QColor(0, col, 255-col)));
          qp->drawRect(OFFSET + i * SQ, OFFSET + j * SQ, SQ, SQ);   
        } else {
          qp->setBrush(QBrush(colors[nodes[i][j].id]));
          qp->drawRect(OFFSET + i * SQ, OFFSET + j * SQ, SQ, SQ);   
        }
      }
  }
}

void MultiscaleWidget::clearNext()
{
  for(int i=0; i<DIM; i++)
    for(int j=0; j<DIM; j++)
    {
      next[i][j].id = 0;
      next[i][j].energy = 0.0f;
    }
}