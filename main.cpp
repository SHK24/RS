#include "mainwindow.h"

#include <QApplication>
#include "jsonexchanger.h"

#include "framelesswindow.h"
#include "DarkStyle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // style our application with custom dark style
    QApplication::setStyle(new DarkStyle);

    // create frameless window (and set windowState or title)
    FramelessWindow framelessWindow;

    //MainWindow w;

    // create our mainwindow instance
    MainWindow *w = new MainWindow;

    framelessWindow.setContent(w);
    framelessWindow.setWindowState(Qt::WindowMaximized);
    framelessWindow.show();

    //w->setWindowState(Qt::WindowMaximized);
    //w->show();
    return a.exec();
}
