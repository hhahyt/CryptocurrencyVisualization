#include "mainwindow.h"
#include <QApplication>
#include <QtCharts>
#include <QLineSeries>
#include <QSplashScreen>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QPixmap>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen splashScreen;
    splashScreen.setPixmap(QPixmap(":/ico/splashScreen.jpg"));
    splashScreen.show();
    splashScreen.showMessage("Updating From Server......", Qt::AlignBottom | Qt::AlignLeft, Qt::white);
    //a.processEvents();
    MainWindow w;

    w.setWindowTitle("Cryptocurrency Visualization");
    //w.resize(1366, 768);
    //w.show();
    splashScreen.finish(&w);
    //![5]
    return a.exec();
}
