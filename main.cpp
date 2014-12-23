#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     ImageViewer imageViewer;

     imageViewer.showMaximized();
     return app.exec();
 }
