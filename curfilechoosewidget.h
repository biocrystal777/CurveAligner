#ifndef CURFILECHOOSEWIDGET_H
#define CURFILECHOOSEWIDGET_H

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>
#include "ffflog.h"
//#include <qwt_global>

#if QWT_VERSION == 0x060102
#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"
#endif

class CurFileChooseWidget : public QFrame
{
   Q_OBJECT
public:
   explicit CurFileChooseWidget(QWidget *parent = nullptr);
   ~CurFileChooseWidget();

    CurFileChooseWidget(const CurFileChooseWidget& src) = delete;
    CurFileChooseWidget& operator= (CurFileChooseWidget& src) = delete;
    CurFileChooseWidget(CurFileChooseWidget&& src) = delete;
    CurFileChooseWidget& operator= (CurFileChooseWidget&& src) = delete;

   QString getOutputFilePath(bool quoted = false);
   QString getInputFilePath1(bool quoted = false);
   QString getInputFilePath2(bool quoted = false);

   bool setInputFilePath1(QString path, bool quoted = true);
   bool setInputFilePath2(QString path, bool quoted = true);
   void setOutputFilePath(QString path, bool quoted);

   void writeSettings();
protected:

   QGridLayout *widgetLayout;

   QGridLayout *fileLayout;
   QToolButton *inputFileChooser1;
   QLineEdit *inputFileName1;
   QToolButton *inputFileChooser2;
   QLineEdit *inputFileName2;

   QToolButton *outputFileChooser;
   QLineEdit *outputFileName;

   /*!
    * \brief chopStringsQuotMarksToOne if a string has more than one Quotation mark, they are merged
    * \example """this string"""   =>    "this string"
    * \param string to be processed
    * \return processed string
    */
   QString chopStringsQuotMarksToOne(QString &string) const;

   /*!
    * \brief chopStringsQuotMarksEntirely removes quotation marks of strings
    * \example """this string"""   =>    this string
    * \param string to be processed
    * \return processed string
    */
   QString chopStringsQuotMarksEntirely(QString &string) const;

protected slots:
   /*!
    * \brief chooseInputFile opens a dialog to choose an input data file
    *        (csv-formatted)
    */
   void chooseInputFile1();

   /*!
    * \brief chooseInputFile opens a dialog to choose an input data file
    *        (csv-formatted)
    */
   void chooseInputFile2();

   /*!
    * \brief chooseOutputFile opens a dialog to choose a folder and filename
    *                         for the output file
    */
   void chooseOutputFile();


};

#endif // CURFILECHOOSEWIDGET_H
