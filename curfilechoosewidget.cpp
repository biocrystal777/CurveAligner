#include "curfilechoosewidget.h"

CurFileChooseWidget::CurFileChooseWidget( QWidget *parent) :
   QFrame(parent)
{
   QSettings settings("AgCoelfen", "CurFileChooseWidget");
   settings.setIniCodec("UTF-8");
   QString stringValue;

   this->setFrameStyle(0x1011);
   fileLayout = new QGridLayout(this);
   fileLayout->addWidget(new QLabel("<b>Curve File 1</b>", this), 0, 1, 1, 3, Qt::AlignBottom);
   inputFileChooser1 = new QToolButton(this);
   inputFileChooser1->setText("..");
   inputFileChooser1->setToolTip("Browse Files");
   QObject::connect(inputFileChooser1, SIGNAL(clicked()), this, SLOT(chooseInputFile1()));
   fileLayout->addWidget(inputFileChooser1, 1, 0, 1, 1);
   inputFileName1 = new QLineEdit(this);
   inputFileName1->setMinimumWidth(300);
   inputFileName1->setMaximumWidth(800);
   fileLayout->addWidget(inputFileName1, 1, 1, 1, 12);
   stringValue = settings.value(tr("fileNames/inputFileName1"), "").toString();
   setInputFilePath1(stringValue, true);

   fileLayout->addWidget(new QLabel("<b>Curve File 2</b>", this), 2, 1, 1, 3, Qt::AlignBottom);
   inputFileChooser2 = new QToolButton(this);
   inputFileChooser2->setText("..");
   inputFileChooser2->setToolTip("Browse Files");
   QObject::connect(inputFileChooser2, SIGNAL(clicked()), this, SLOT(chooseInputFile2()));
   fileLayout->addWidget(inputFileChooser2, 3, 0, 1, 1);
   inputFileName2 = new QLineEdit(this);
   inputFileName2->setMinimumWidth(300);
   inputFileName2->setMaximumWidth(800);
   fileLayout->addWidget(inputFileName2, 3, 1, 1, 12);
   stringValue = settings.value(tr("fileNames/inputFileName2"), "").toString();
   setInputFilePath2(stringValue, true);

   fileLayout->addWidget(new QLabel("<b>Data Output File</b>", this), 5, 1, 1, 3, Qt::AlignBottom);
   outputFileChooser = new QToolButton(this);
   outputFileChooser->setText("..");
   outputFileChooser->setToolTip("Browse Files");
   QObject::connect(outputFileChooser, SIGNAL(clicked()), this, SLOT(chooseOutputFile()));
   fileLayout->addWidget(outputFileChooser, 6, 0, 1, 1);
   outputFileName = new QLineEdit(this);
   outputFileName->setMinimumWidth(300);
   outputFileName->setMaximumWidth(800);
   fileLayout->addWidget(outputFileName, 6, 1, 1, 12);
   stringValue = settings.value(tr("fileNames/outputFileName"), "").toString();
   setOutputFilePath(stringValue, true);

}

CurFileChooseWidget::~CurFileChooseWidget()
{
    writeSettings();
}


QString CurFileChooseWidget::getOutputFilePath(bool quoted)
{
      QString path = outputFileName->text();
      if(quoted)
         chopStringsQuotMarksToOne(path);
      else
         chopStringsQuotMarksEntirely(path);
      return path;
}

QString CurFileChooseWidget::getInputFilePath1(bool quoted)
{
   QString path = inputFileName1->text();
   if(quoted)
      chopStringsQuotMarksToOne(path);
   else
      chopStringsQuotMarksEntirely(path);
   return path;
}

QString CurFileChooseWidget::getInputFilePath2(bool quoted)
{
   QString path = inputFileName2->text();
   if(quoted)
      chopStringsQuotMarksToOne(path);
   else
      chopStringsQuotMarksEntirely(path);
   return path;
}

bool CurFileChooseWidget::setInputFilePath1( QString path, bool quoted)
{
   bool ok(true);
   QString testPath = path;
   chopStringsQuotMarksEntirely(testPath);
   if(!QFile::exists(testPath)){
      path = QDir::homePath();
      path.prepend('"');
      path.append('"');
      ok = false;
   }
   else {
      if(quoted) chopStringsQuotMarksToOne(path);
      else chopStringsQuotMarksEntirely(path);
      inputFileName1->setText(path);
   }
   return ok;
}

bool CurFileChooseWidget::setInputFilePath2( QString path, bool quoted)
{
   bool ok(true);
   QString testPath = path;
   chopStringsQuotMarksEntirely(testPath);
   if(!QFile::exists(testPath)){
      path = QDir::homePath();
      path.prepend('"');
      path.append('"');
      ok = false;
   }
   else {
      if(quoted) chopStringsQuotMarksToOne(path);
      else chopStringsQuotMarksEntirely(path);
      inputFileName2->setText(path);
   }
   return ok;
}

void CurFileChooseWidget::setOutputFilePath(QString path, bool quoted)
{
   if(quoted) chopStringsQuotMarksToOne(path);
   else chopStringsQuotMarksEntirely(path);
   outputFileName->setText(path);
}

void CurFileChooseWidget::writeSettings()
{
   QSettings settings("AgCoelfen", "CurFileChooseWidget");
   settings.setIniCodec("UTF-8");
   settings.setValue(tr("fileNames/inputFileName1"), getInputFilePath1(true));
   settings.setValue(tr("fileNames/inputFileName2"), getInputFilePath2(true));
   settings.setValue(tr("fileNames/outputFileName"), getOutputFilePath(true));
}

void CurFileChooseWidget::chooseInputFile1()
{
   QString s;
   QString oldInputFile = inputFileName1->text();
   if(!QFile::exists(oldInputFile)) oldInputFile = QDir::homePath();
   s = QFileDialog::getOpenFileName(this, tr("Choose a File to evaluate"), oldInputFile,
                                    QString(), nullptr,
                                    ( QFileDialog::DontConfirmOverwrite |
                                      QFileDialog::ReadOnly |
                                      QFileDialog::HideNameFilterDetails )
                                    );
   if(QFile::exists(s)) inputFileName1->setText(chopStringsQuotMarksToOne(s));
   else FFFLog::logWarning(tr("Chosen input file does not exist."));
}

void CurFileChooseWidget::chooseInputFile2()
{
   QString s;
   QString oldInputFile = inputFileName2->text();
   if(!QFile::exists(oldInputFile)) oldInputFile = QDir::homePath();
   s = QFileDialog::getOpenFileName(this, tr("Choose a File to evaluate"), oldInputFile,
                                    QString(), nullptr,
                                    ( QFileDialog::DontConfirmOverwrite |
                                      QFileDialog::ReadOnly |
                                      QFileDialog::HideNameFilterDetails )
                                    );
   if(QFile::exists(s)) inputFileName2->setText(chopStringsQuotMarksToOne(s));
   else FFFLog::logWarning(tr("Chosen input file does not exist."));
}

void CurFileChooseWidget::chooseOutputFile()
{
   QString oldOutputFile = outputFileName->text();
   // ehb: I replaced this with a function to use the inputFile path if the oldOutputFile is a directory
   // if(!QFile::exists(oldOutputFile)) oldOutputFile = QDir::homePath();
   if(!QFile::exists(oldOutputFile) || QFileInfo( oldOutputFile ).isDir() ) oldOutputFile = QFileInfo( inputFileName1->text() ).path();
   QFileDialog dialog(this, tr("Choose an file Name for your results,"), oldOutputFile);
   dialog.setFileMode(QFileDialog::AnyFile);
   dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
   QObject::connect(&dialog, SIGNAL(fileSelected(QString)), outputFileName, SLOT(setText(QString)));
   dialog.exec();
   QString s = outputFileName->text();
   chopStringsQuotMarksToOne(s);
   outputFileName->setText(s);
}

QString CurFileChooseWidget::chopStringsQuotMarksToOne(QString & string) const
{
#if QT_VERSION >= 0x05000
   chopStringsQuotMarksEntirely(string);
   string.prepend('\"');
   string.append('\"');
#else
   const QChar compChar('\"');
   uint stringLength = string.length();
   if(stringLength < 5)
      return QString("");
   while(string.at(1) == compChar){
      string.remove(0, 1);
   }
   stringLength = string.length();

   while(string.at(stringLength - 2).toAscii() == compChar.toAscii()){
      string.remove(stringLength - 1, 1);
      stringLength = string.length();
   }
   stringLength = string.length();
   if(string.at(0).toAscii() != compChar.toAscii()) string.prepend(compChar);
   if(string.at(stringLength - 1).toAscii() != compChar.toAscii()) string.append((compChar));
#endif
   return string;
}

QString CurFileChooseWidget::chopStringsQuotMarksEntirely(QString & string) const
{
#if QT_VERSION >= 0x05000
   const QChar quotMark('\"');
   while(!string.isEmpty() && string.at(0) == quotMark)
      string.remove(0, 1);
   while(!string.isEmpty() && string.at(string.length()-1) == quotMark)
      string.remove(string.length()-1, 1);

#else
   const QChar compChar('\"');
   uint stringLength = string.length();
   int removed=0;
   for(int i = 0; i < stringLength; ++i)
      if(string.at(i-removed).toAscii() == compChar.toAscii()){
         string.remove(i-removed, 1);
         ++removed;
      }
#endif
   return string;
}
