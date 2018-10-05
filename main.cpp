#include "curmainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   CurMainWidget w;
   w.show();




   return a.exec();
}
