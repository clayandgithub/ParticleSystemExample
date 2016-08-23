#include <QtGui/QApplication>
#include <QApplication>
#include "glwidget.h"
#include <iostream>
#include <QtOpenGL>
#include <QPixmap>

using namespace std;
int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QApplication a(argc, argv);
    if (!QGLFormat::hasOpenGL())
    {
        cerr << "This system has no OpenGL support" << endl;
        return 1;
    }
    GLWidget w;
    w.setWindowTitle(QObject::tr("WWW"));
    w.setGeometry(100,100,500,500);
    w.resize(500, 500);
    w.show();
    
    return a.exec();
}
