#include "FTP_client.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>
#include <qstylefactory.h>


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP1251")); 
    QApplication a(argc, argv);
    FTP_client w;
    w.show();
    return a.exec();
}




















