#ifndef CURMAINWIDGET_H
#define CURMAINWIDGET_H

#include "./ffflog.h"
#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include "./curfilechoosewidget.h"
#include "./fffcsvparser.h"
#include <QDoubleSpinBox>
#include <QCheckBox>

class CurMainWidget final : public QWidget
{
   Q_OBJECT

public:
   CurMainWidget(QWidget *parent = 0);
   ~CurMainWidget();


   CurMainWidget(const CurMainWidget& src) = delete;
   CurMainWidget& operator= (CurMainWidget& src) = delete;
   CurMainWidget(CurMainWidget&& src) = delete;
   CurMainWidget& operator= (CurMainWidget&& src) = delete;

private:

   QGridLayout *lay                                    = nullptr;
   FFFLog *logWidget                                   = nullptr;
   CurFileChooseWidget *fileWidget                     = nullptr;


   QwtPlot                        *signalPlot          = nullptr;
   QwtPlotGrid                    *signalGrid          = nullptr;
   QwtPlotCurve                   *signalCurve1        = nullptr;
   QwtPlotCurve                   *signalCurve2        = nullptr;

   QwtPlot                        *pathPlot            = nullptr;
   QwtPlotGrid                    *pathGrid            = nullptr;
   QwtPlotCurve                   *pathCurve           = nullptr;




   QPushButton                    *aligner             = nullptr;

   QDoubleSpinBox                 *curve1LeftCutBox    = nullptr;
   QDoubleSpinBox                 *curve1RightCutBox   = nullptr;
   QDoubleSpinBox                 *curve2LeftCutBox    = nullptr;
   QDoubleSpinBox                 *curve2RightCutBox   = nullptr;

   QCheckBox                      *curve1Inverter      = nullptr;
   QCheckBox                      *curve2Inverter      = nullptr;
   QCheckBox                      *curve1Normalizer    = nullptr;
   QCheckBox                      *curve2Normalizer    = nullptr;



private slots:

   void checkBox1Left(double value);
   void checkBox1Right(double value);
   void checkBox2Left(double value);
   void checkBox2Right(double value);

   void align();
private:


   QGroupBox                      *modelGroup       = nullptr;
   QGridLayout                    *modelGroupLay    = nullptr;
   QRadioButton                   *euclidButton     = nullptr;
   QRadioButton                   *manhattButton    = nullptr;
   QRadioButton                   *chebyshevButton  = nullptr;


   void replot(const QVector<double> &x1,
               const QVector<double> &y1,
               const QVector<double> &x2,
               const QVector<double> &y2);


   void normalize(vecD &v);
   matD transpose(const matD &in) const;
   matD cutEdges(const matD &mat, const double left, const double right) const;

   //void normalizeDataToMaximum(std::vector<double> &vec);

};

#endif // CURMAINWIDGET_H
