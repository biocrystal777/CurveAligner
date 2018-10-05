#include "./curmainwidget.h"
#include <algorithm>
#include "./us_multi_column.h"
#include "./interpolate.h"
#include <aquila/ml/Dtw.h>
#include "./fffcsvwriter.h"
#include <QCheckBox>

using std::vector;
using std::string;

CurMainWidget::CurMainWidget(QWidget *parent)
   : QWidget(parent)
{

   this->setMinimumSize(800, 600);

   lay = new QGridLayout(this);

   fileWidget = new CurFileChooseWidget(this);
   lay->addWidget(fileWidget, 0, 0, 3, 10);



   signalPlot = new QwtPlot(this);
   lay->addWidget(signalPlot, 4, 0, 5, 5);
   signalPlot->setCanvasBackground(Qt::white);
   signalPlot->enableAxis(QwtPlot::xTop, true);
   signalPlot->setAxisTitle(QwtPlot::xTop, tr( "X 1 " ) );
   signalPlot->setAxisTitle(QwtPlot::xBottom, tr( "X 2 " ) );
   signalPlot->setAxisTitle(QwtPlot::yLeft, tr( "Y" ) );
   //signalPlot->setTitle();
   signalGrid = new QwtPlotGrid;
   signalGrid->enableX(false);
   //signalGrid->enableXMin(true);
   //signalGrid->enableYMin(true);
   signalGrid->setMajorPen( QPen( Qt::lightGray, 0, Qt::SolidLine ));
   signalGrid->setMinorPen( QPen( Qt::white, 0, Qt::DotLine ) );
   signalGrid->attach(signalPlot);


   pathPlot = new QwtPlot(this);
   lay->addWidget(pathPlot, 4, 5, 5, 5);
   pathPlot->setCanvasBackground(Qt::white);
   //pathPlot->enableAxis(QwtPlot::xTop, true);
   pathPlot->setAxisTitle(QwtPlot::xBottom, tr( "X 1 " ) );
   pathPlot->setAxisTitle(QwtPlot::yLeft, tr( "Y" ) );
   //pathPlot->setTitle();
   pathGrid = new QwtPlotGrid;
   //pathGrid->enableX(false);
   //pathGrid->enableXMin(true);
   //pathGrid->enableYMin(true);
   pathGrid->enableX(true);
   pathGrid->setMajorPen( QPen( Qt::lightGray, 0, Qt::SolidLine ));
   //pathGrid->setMinorPen( QPen( Qt::white, 0, Qt::DotLine ) );
   pathGrid->attach(pathPlot);

   aligner = new QPushButton("Align", this);
   QObject::connect(aligner, SIGNAL(clicked()), this, SLOT(align()));

   lay->addWidget(aligner, 9, 2, 2, 1, Qt::AlignCenter);

   curve1LeftCutBox = new QDoubleSpinBox(this);
   curve1LeftCutBox->setMinimum(0.0);
   curve1LeftCutBox->setMaximum(100.0);
   curve1LeftCutBox->setValue(0.0);
   curve1LeftCutBox->setDecimals(1);
   //qDebug() << curve1LeftCutBox->backgroundRole();
   //curve1LeftCutBox->setBackgroundRole(QPalette::Base);
   //curve1LeftCutBox->setAutoFillBackground(true);
   lay->addWidget(new QLabel("Cut left [%]:", this), 9, 3, 1, 1);
   lay->addWidget(curve1LeftCutBox, 9, 4, 1, 1);
   curve1RightCutBox = new QDoubleSpinBox(this);
   curve1RightCutBox->setMinimum(0);
   curve1RightCutBox->setMaximum(100);
   curve1RightCutBox->setValue(100.0);
   curve1RightCutBox->setDecimals(1);
   lay->addWidget(new QLabel("Cut right [%]:", this), 9, 5, 1, 1);
   lay->addWidget(curve1RightCutBox, 9, 6, 1, 1);
   curve1Inverter = new QCheckBox("Invert Axis", this);
   lay->addWidget(curve1Inverter, 9, 7, 1, 2);
   curve1Normalizer = new QCheckBox("Normalize", this);
   lay->addWidget(curve1Normalizer, 9, 9, 1, 2);

   curve2LeftCutBox = new QDoubleSpinBox(this);
   curve2LeftCutBox->setMinimum(0.0);
   curve2LeftCutBox->setMaximum(100.0);
   curve2LeftCutBox->setValue(0.0);
   curve2LeftCutBox->setDecimals(1);
   lay->addWidget(new QLabel("Cut left [%]:", this), 10, 3, 1, 1);
   lay->addWidget(curve2LeftCutBox, 10, 4, 1, 1);
   curve2RightCutBox = new QDoubleSpinBox(this);
   curve2RightCutBox->setMinimum(0);
   curve2RightCutBox->setMaximum(100);
   curve2RightCutBox->setValue(100.0);
   curve2RightCutBox->setDecimals(1);
   lay->addWidget(new QLabel("Cut right [%]:", this), 10, 5, 1, 1);
   lay->addWidget(curve2RightCutBox, 10, 6, 1, 1);
   curve2Inverter = new QCheckBox("Invert Axis",this);
   lay->addWidget(curve2Inverter, 10, 7, 1, 2);
   curve2Normalizer = new QCheckBox("Normalize", this);
   lay->addWidget(curve2Normalizer, 10, 9, 1, 2);

   QObject::connect(curve1LeftCutBox,  SIGNAL(valueChanged(double)), this, SLOT(checkBox1Left(double)));
   QObject::connect(curve1RightCutBox, SIGNAL(valueChanged(double)), this, SLOT(checkBox1Right(double)));
   QObject::connect(curve2LeftCutBox,  SIGNAL(valueChanged(double)), this, SLOT(checkBox2Left(double)));
   QObject::connect(curve2RightCutBox, SIGNAL(valueChanged(double)), this, SLOT(checkBox2Right(double)));

   modelGroup = new QGroupBox(this);
   lay->addWidget(modelGroup, 9, 0, 2, 2);
   modelGroupLay = new QGridLayout(modelGroup);
   euclidButton = new QRadioButton("Euclidean",modelGroup);
   modelGroupLay->addWidget(euclidButton,    0, 0, 1, 2);
   manhattButton = new QRadioButton("Manhattan", modelGroup);
   modelGroupLay->addWidget(manhattButton,    1, 0, 1, 2);
   chebyshevButton = new QRadioButton("Chebyshev", modelGroup);
   modelGroupLay->addWidget(chebyshevButton, 2, 0, 1, 2);
   euclidButton->setChecked(true);


   //modelGroup-
   //QRadioButton                   *splineButton    = nullptr;
   //QRadioButton                   *linearButton    = nullptr;
   //QRadioButton                   *quadraticButton = nullptr;

   logWidget = new FFFLog(this);
   lay->addWidget(logWidget, 11, 0, 3, 10);
}

CurMainWidget::~CurMainWidget()
{

}

void CurMainWidget::checkBox1Left(double value)
{
   QPalette pal(curve1LeftCutBox->palette());
   pal.setColor(QPalette::Highlight, Qt::black);
   if(curve1RightCutBox->value() <= value){
      pal.setColor(QPalette::Text, Qt::red);
      pal.setColor(QPalette::HighlightedText, Qt::red);
   }
   else {
      pal.setColor(QPalette::Text, Qt::green);
      pal.setColor(QPalette::HighlightedText, Qt::green);
   }
   curve1LeftCutBox->setPalette(pal);
   curve1RightCutBox->setPalette(pal);
}

void CurMainWidget::checkBox1Right(double value)
{
   QPalette pal(curve1LeftCutBox->palette());
   pal.setColor(QPalette::Highlight, Qt::black);
   if(curve1LeftCutBox->value() >= value){
      pal.setColor(QPalette::Text, Qt::red);
      pal.setColor(QPalette::HighlightedText, Qt::red);
   }
   else {
      pal.setColor(QPalette::Text, Qt::green);
      pal.setColor(QPalette::HighlightedText, Qt::green);
   }
   curve1LeftCutBox->setPalette(pal);
   curve1RightCutBox->setPalette(pal);
}

void CurMainWidget::checkBox2Left(double value)
{
   QPalette pal(curve2LeftCutBox->palette());
   pal.setColor(QPalette::Highlight, Qt::black);
   if(curve2RightCutBox->value() <= value){
      pal.setColor(QPalette::Text, Qt::red);
      pal.setColor(QPalette::HighlightedText, Qt::red);
   }
   else {
      pal.setColor(QPalette::Text, Qt::green);
      pal.setColor(QPalette::HighlightedText, Qt::green);
   }
   curve2LeftCutBox->setPalette(pal);
   curve2RightCutBox->setPalette(pal);
}

void CurMainWidget::checkBox2Right(double value)
{
   QPalette pal(curve2LeftCutBox->palette());
   pal.setColor(QPalette::Highlight, Qt::black);
   if(curve2LeftCutBox->value() >= value){
      pal.setColor(QPalette::Text, Qt::red);
      pal.setColor(QPalette::HighlightedText, Qt::red);
   }
   else {
      pal.setColor(QPalette::Text, Qt::green);
      pal.setColor(QPalette::HighlightedText, Qt::green);
   }
   curve2LeftCutBox->setPalette(pal);
   curve2RightCutBox->setPalette(pal);
}

void CurMainWidget::replot(const QVector<double> &x1,
                           const QVector<double> &y1,
                           const QVector<double> &x2,
                           const QVector<double> &y2)
{

   if(!signalCurve1){
      signalCurve1 = new QwtPlotCurve(QString("Measured"));
      signalCurve1->setStyle(QwtPlotCurve::Lines);
      signalCurve1->setPen( QPen( QBrush(QColor(0x00, 0x66, 0x00)), 2.0, Qt::SolidLine));
      signalCurve1->attach(signalPlot);
      signalCurve1->show();
   }
   signalCurve1->setSamples(x1, y1);

   if(!signalCurve2){
      signalCurve2 = new QwtPlotCurve(QString("Measured"));
      signalCurve2->setStyle(QwtPlotCurve::Lines);
      signalCurve2->setPen( QPen( QBrush(QColor(0x00, 0x66, 0xCC)), 2.0, Qt::SolidLine));
      signalCurve2->attach(signalPlot);
      signalCurve2->show();
   }
   signalCurve2->setSamples(x1, y2); // x1 as axis have been merged

   signalPlot->setAxisScale(QwtPlot::xBottom, x1.first(), x1.last());
   signalPlot->setAxisScale(QwtPlot::xTop,    x2.first(), x2.last());

   if(!pathCurve){
      pathCurve = new QwtPlotCurve(QString("Alignment Path"));
      pathCurve->setStyle(QwtPlotCurve::Lines);
      pathCurve->setPen( QPen( QBrush(QColor(0x2E, 0x66, 0x4C)), 2.0, Qt::SolidLine));
      pathCurve->attach(pathPlot);
      pathCurve->show();
   }
   pathCurve->setSamples(x1, x2); // x1 as axis have been merged

   pathPlot->setAxisScale(QwtPlot::xBottom, x1.first(), x1.last());
   pathPlot->setAxisScale(QwtPlot::yLeft,   x2.first(), x2.last());

   signalPlot->replot();
   pathPlot->replot();
}

void CurMainWidget::normalize(vecD &v)
{   
   const double minV = *std::min_element(v.begin(), v.end());
   //qDebug() << minV;
   for(double &val : v) val -= minV;
 //  qDebug() << "MIN" << *std::min_element(v.begin(), v.end());
   const double maxV = *std::max_element(v.begin(), v.end()) / 10000;
   //qDebug() << "MAX" <<  maxV;
   for(double &val : v) val /= maxV;
   //qDebug() << *std::min(v.begin(), v.end());
}

matD CurMainWidget::transpose(const matD &in) const
{
   if(in.size() == 0 || in[0].size() == 0 ){
      return matD(0);
   }

   for(const vector<double> &vec : in){
      if(vec.size() != in[0].size()) return matD(0);
   }
   //qDebug() << "MARK 1";
   //vector<uint> xSize(in.size());
   //for(uint i = 0; i < xSizes.size(); ++i) xSizes[i]  = in[i].size();
   //const uint xSize =  *std::max_element(xSizes.begin(), xSizes.end());
   const uint xSize = in[0].size();
   //qDebug() << "MARK 2" << xSize;
   matD out(xSize);
   for(vector<double> &vec : out) vec = vector<double>(in.size());
   //qDebug() << "MARK 3" << xSize;
   for(uint i = 0; i < xSize; ++i){
      for(uint j = 0; j < out[i].size(); ++j){
         out[i][j] = in[j][i];
      }
   }

   return out;
}

matD CurMainWidget::cutEdges(const matD &mat, const double left, const double right) const
{
   if(left > 0.0 || right < 100.0){
      const uint tmpFirst = static_cast<uint>( static_cast<double>(mat.size() - 1) * (left / 100.0) );
      const uint tmpLast  = static_cast<uint>( static_cast<double>(mat.size() - 1) * (right / 100.0) );
      const uint newSize = tmpLast - tmpFirst + 1;
      //qDebug() << "CUT" << tmpFirst << tmpLast << newSize;
      matD tmp(newSize) ;
      uint c = 0;
      for(uint i = tmpFirst; i <= tmpLast ; ++i){
         tmp[c] = mat[i];
         ++c;
      }
      return tmp;
   }
   else return mat;

}
/*
void CurMainWidget::normalizeDataToMaximum(std::vector<double> &vec)
{
   const double maxValue = *(std::max_element(vec.begin(), vec.end()));
   for(double &val : vec) val /= maxValue;
}
*/

//return xSizes[i];
   //sizes
   //for(sizes)
   //uint size()

//matD out(5);
//}


void CurMainWidget::align()
{
   QString inFile1 = fileWidget->getInputFilePath1();
   QString inFile2 = fileWidget->getInputFilePath2();
   QString outFile = fileWidget->getOutputFilePath();

   if(!QFile::exists(inFile1)){
      FFFLog::logWarning(tr("File with path %1 does not exist.").arg(inFile1));
      return;
   }

   if(!QFile::exists(inFile2)){
      FFFLog::logWarning(tr("File with path %1 does not exist.").arg(inFile2));
      return;
   }



   // Parse input Files

   FFFCsvParser parser1(inFile1.toStdString(), ',', '.');
   parser1.parseFile();
   vector<string> headLines1 = parser1.getHeadLines();
   matD data1 = parser1.getData();
   FFFCsvParser parser2(inFile2.toStdString(), ',', '.');
   parser2.parseFile();
   vector<string> headLines2 = parser2.getHeadLines();
   matD data2 = parser2.getData();
   if(data1.size() != 2){
      FFFLog::logText(tr("File %1 has invalid format.").arg(inFile1));
      return;
   }
   if(data2.size() != 2){
      FFFLog::logText(tr("File %1 has invalid format.").arg(inFile2));
      return;
   }

   if(curve1Inverter->isChecked()){
      std::reverse(data1[0].begin(), data1[0].end());
      std::reverse(data1[1].begin(), data1[1].end());
   }

   if(curve2Inverter->isChecked()){
      std::reverse(data2[0].begin(), data2[0].end());
      std::reverse(data2[1].begin(), data2[1].end());
   }

   matD normData1 = data1;
   matD normData2 = data2;

   if(curve1Normalizer->isChecked()){
      normalize(normData1[1]);
   }

   if(curve2Normalizer->isChecked()){
      normalize(normData2[1]);
   }

   matD dataT1 = transpose(normData1);
   matD dataT2 = transpose(normData2);

   // qDebug() << dataT1.size();
   data1 = cutEdges(dataT1, curve1LeftCutBox->value(), curve1RightCutBox->value());
   //qDebug() << curve2LeftCutBox->value() << curve2RightCutBox->value();
   data2 = cutEdges(dataT2, curve2LeftCutBox->value(), curve2RightCutBox->value());





   for(uint i = 0; i < dataT2.size(); ++i)
   {
      qDebug() << dataT2[i][1];
   }

   //qDebug() << dataT1.size() << dataT2.size();
   /*
   if(curve1LeftCutBox->value() > 0.0 || curve1RightCutBox->value() < 100.0){
      const uint tmpFirst = static_cast<uint>( static_cast<double>(dataT1.size() - 1) * (curve1LeftCutBox->value()  / 100.0) );
      const uint tmpLast  = static_cast<uint>( static_cast<double>(dataT1.size() - 1) * (curve1RightCutBox->value() / 100.0) );
      const uint newSize = tmpLast - tmpFirst + 1;
      matD tmp(newSize) ;
      uint c = 0;
      for(uint i = tmpFirst; i <= tmpLast ; ++i){
         tmp[c] = dataT1[i];
         ++c;
      }
   }
*/

   //qDebug() << "MARK 2";

   QString distType; // algorithmType;
   //double distance;
   Aquila::Dtw dtw;
   if(euclidButton->isChecked()){
      dtw = Aquila::Dtw(Aquila::euclideanDistance, Aquila::Dtw::Neighbors);
      distance = dtw.getDistance(dataT1, dataT2);
      distType = "Euclidean";
   }
   else if(manhattButton->isChecked()){
      dtw = Aquila::Dtw(Aquila::manhattanDistance, Aquila::Dtw::Neighbors);
      distance = dtw.getDistance(dataT1, dataT2);
      distType = "Manhattan";
   }
   else if(chebyshevButton->isChecked()){
      dtw = Aquila::Dtw(Aquila::chebyshevDistance, Aquila::Dtw::Neighbors);
      distance = dtw.getDistance(dataT1, dataT2);
      distType = "Chebyshev";
   }


   Aquila::DtwPathType path = dtw.getPath();

   qDebug() << dataT1.size() << dataT2.size() << path.size();

   QVector<double> showDataX1(path.size());
   QVector<double> showDataY1(path.size());
   QVector<double> showDataX2(path.size());
   QVector<double> showDataY2(path.size());

   matD cutData1 = transpose(dataT1);
   matD cutData2 = transpose(dataT2);
   //qDebug() << data1[0].size() << cutData1[0].size();

   for(uint i = 0; i < path.size(); ++i){
      Aquila::DtwPoint pathP = path[i];
      showDataX1[i] = cutData1[0][pathP.x];
      showDataY1[i] = cutData1[1][pathP.x];
      showDataX2[i] = cutData2[0][pathP.y];
      showDataY2[i] = cutData2[1][pathP.y];
   }
   std::reverse(showDataX1.begin(), showDataX1.end());
   std::reverse(showDataY1.begin(), showDataY1.end());
   std::reverse(showDataX2.begin(), showDataX2.end());
   std::reverse(showDataY2.begin(), showDataY2.end());

   // write Path to output file

/*
   FFFCsvWriter writer(outFile.toStdString());
   writer.writeFile(matD({showDataX1.toStdVector(), showDataX2.toStdVector()}),
                    vector<string>({headLines1[0], headLines2[0]}));
*/
   FFFCsvWriter writer2(outFile.append("_2").toStdString());
   writer2.writeFile(matD({showDataX1.toStdVector(), showDataY1.toStdVector(), showDataX2.toStdVector(), showDataY2.toStdVector()}),
                     vector<string>({headLines1[0], headLines1[1],headLines2[0], headLines2[1]}));


   replot(showDataX1,showDataY1,showDataX2,showDataY2);

   FFFLog::logText(tr("Finished with a total distance of %1 (%2)").arg(distance).arg(distType));

   //int errorbase;






   /*
    *
    *
    */

   /* // EMRE KRAM


   uint dep1 = 0;
   uint indep1 =1;
   uint dep2 = 0;
   uint indep2 =1;

   qDebug() << " MARK 1";

   US_Multi_Column mc1( infile1 );
   US_Multi_Column mc2( infile2 );
   if ( !mc1.read() )
   {
      FFFLog::logError(mc1.errormsg);
      return;
      //qDebug() << mc1.errormsg << endl;
      //exit( errorbase );
   }
   //errorbase--;
   if ( !mc2.read() )
   {
      qDebug() << mc2.errormsg << endl;
     // exit( errorbase );
   }
   //errorbase--;
   qDebug() << mc1.info();
   qDebug() << mc2.info();
   qDebug() << " MARK 2";
   US_Multi_Column mc1_asc;
   US_Multi_Column mc1_des;

   US_Multi_Column mc2_asc;
   US_Multi_Column mc2_des;

   if ( !mc1.split_on_peak( mc1_asc,
                            mc1_des,
                            indep1,
                            dep1 ) ||
        !mc2.split_on_peak( mc2_asc,
                            mc2_des,
                            indep2,
                            dep2 ) )
   {
      qDebug() << mc1.errormsg << endl;
      qDebug() << mc2.errormsg << endl;
      //exit( errorbase );
   }
   //errorbase--;
   qDebug() << " MARK 3";
   qDebug() << mc1_asc.info();
   qDebug() << mc1_des.info();
   qDebug() << mc2_asc.info();
   qDebug() << mc2_des.info();

   US_Multi_Column mc1_asc_mono;
   US_Multi_Column mc1_des_mono;

   US_Multi_Column mc2_asc_mono;
   US_Multi_Column mc2_des_mono;
   qDebug() << " MARK 4";
   if ( !mc1_asc.monotonize( mc1_asc_mono, dep1 ) ||
        !mc1_des.monotonize( mc1_des_mono, dep1 ) ||
        !mc2_asc.monotonize( mc2_asc_mono, dep2 ) ||
        !mc2_des.monotonize( mc2_des_mono, dep2 ) )
   {
      qDebug() << mc1_asc.errormsg << endl;
      qDebug() << mc1_des.errormsg << endl;
      qDebug() << mc2_asc.errormsg << endl;
      qDebug() << mc2_des.errormsg << endl;
     // exit( errorbase );
   }
   //errorbase--;

   qDebug() << mc1_asc_mono.info();
   qDebug() << mc1_des_mono.info();
   qDebug() << mc2_asc_mono.info();
   qDebug() << mc2_des_mono.info();

   US_Multi_Column mc1_mono_joined;
   US_Multi_Column mc2_mono_joined;

   if ( !mc1_mono_joined.join( mc1_asc_mono, mc1_des_mono, "mc1_mono_joined.txt" ) ||
        !mc2_mono_joined.join( mc2_asc_mono, mc2_des_mono, "mc2_mono_joined.txt" ) )
   {
      qDebug() << mc1_mono_joined.errormsg << endl;
      qDebug() << mc2_mono_joined.errormsg << endl;
 //     exit( errorbase );
   }
   //errorbase--;

   qDebug() << mc1_mono_joined.info();
   qDebug() << mc2_mono_joined.info();

//       US_Multi_Column result_spline_asc;
//       US_Multi_Column result_spline_des;
//       US_Multi_Column result_spline_join;

//       if (
//           !result_spline_asc.spline( mc1_asc,
//                                      mc2_asc_mono,
//                                      dep1,
//                                      indep2,
//                                      dep2,
//                                      "spline-asc.txt"
//                                      ) ||
//           !result_spline_des.spline( mc1_des,
//                                      mc2_des_mono,
//                                      dep1,
//                                      indep2,
//                                      dep2,
//                                      "spline-des.txt"
//                                      ) ||
//           !result_spline_join.join( result_spline_asc, result_spline_des, outfile + "-spline.txt" )
//           )
//       {
//          qDebug() << result_spline_asc.errormsg << endl;
//          qDebug() << result_spline_des.errormsg << endl;
//          qDebug() << result_spline_join.errormsg << endl;
//          exit( errorbase );
//       }
//      errorbase--;

//       US_Multi_Column result_quadratic_asc;
//       US_Multi_Column result_quadratic_des;
//       US_Multi_Column result_quadratic_join;

//       if (
//           !result_quadratic_asc.quadratic( mc1_asc,
//                                            mc2_asc_mono,
//                                            dep1,
//                                            indep2,
//                                            dep2,
//                                            "quadratic-asc.txt"
//                                            ) ||
//           !result_quadratic_des.quadratic( mc1_des,
//                                            mc2_des_mono,
//                                            dep1,
//                                            indep2,
//                                            dep2,
//                                            "quadratic-des.txt"
//                                            ) ||
//           !result_quadratic_join.join( result_quadratic_asc, result_quadratic_des, outfile + "-quadratic.txt" )
//           )
//       {
//          qDebug() << result_quadratic_asc.errormsg << endl;
//          qDebug() << result_quadratic_des.errormsg << endl;
//          qDebug() << result_quadratic_join.errormsg << endl;
//          exit( errorbase );
//       }
//       errorbase--;

   US_Multi_Column result_linear_asc;
   US_Multi_Column result_linear_des;
   US_Multi_Column result_linear_join;

   if (
       !mc1_asc.write( "", true ) ||
       !mc1_des.write( "", true ) ||
       !mc2_asc.write( "", true ) ||
       !mc2_des.write( "", true ) ||
       !mc1_asc_mono.write( "", true ) ||
       !mc1_des_mono.write( "", true ) ||
       !mc2_asc_mono.write( "", true ) ||
       !mc2_des_mono.write( "", true ) ||
       !mc1_mono_joined.write( "", true ) ||
       !mc2_mono_joined.write( "", true ) )
   {
      qDebug() << mc1.errormsg << endl;
      qDebug() << mc2.errormsg << endl;
     // exit( errorbase );
   }
   //errorbase--;

   if (
       !result_linear_asc.linear( mc1_asc,
                                  mc2_asc_mono,
                                  dep1,
                                  indep2,
                                  dep2,
                                  "linear-asc.txt"
                                  ) ||
       !result_linear_des.linear( mc1_des,
                                  mc2_des_mono,
                                  dep1,
                                  indep2,
                                  dep2,
                                  "linear-des.txt"
                                  ) ||
       !result_linear_join.join( result_linear_asc, result_linear_des, outfile  + "-linear.txt" )
       )
   {
      qDebug() << result_linear_asc.errormsg << endl;
      qDebug() << result_linear_des.errormsg << endl;
      qDebug() << result_linear_join.errormsg << endl;
      //exit( errorbase );
   }
   //errorbase--;

   US_Multi_Column result_linear_join_csv = result_linear_join;
   result_linear_join_csv.filename.replace( QRegExp( "\\.txt$" ), ".csv" );

   if (
       // !result_spline_asc.write( "", true ) ||
       // !result_spline_des.write( "", true ) ||
       // !result_spline_join.write( "", true ) ||
       // !result_quadratic_asc.write( "", true ) ||
       // !result_quadratic_des.write( "", true ) ||
       // !result_quadratic_join.write( "", true ) ||
       !result_linear_asc     .write( "", true ) ||
       !result_linear_des     .write( "", true ) ||
       !result_linear_join    .write( "", true ) ||
       !result_linear_join_csv.write( "", true )
       )
   {
      qDebug() << mc1.errormsg << endl;
      qDebug() << mc2.errormsg << endl;
      //exit( errorbase );
   }

*/



}



