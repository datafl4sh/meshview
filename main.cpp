#include <QtGui>

#include <iostream>

#include "MainWindow.h"
#include "Mesh.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    MainWindow mw;
    mw.show();
    
    //QColorDialog qcd;
    //qcd.show();
    
    return app.exec();
}