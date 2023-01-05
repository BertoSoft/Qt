#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow      w;
    QImage          img_splahs;
    QPixmap         pix_splash;
    QSplashScreen   frm_splash;

    //
    // Cargo la imagen del splash
    //
    img_splahs.load(":/imagenes/cargadora3.jpeg");
    img_splahs = img_splahs.scaled(400, 300, Qt::KeepAspectRatio);

    pix_splash = QPixmap::fromImage(img_splahs);

    frm_splash.setPixmap(pix_splash);
    frm_splash.show();
    frm_splash.showMessage("Espere... ", Qt::AlignBottom);

    //
    // Terminamos el splash y cargamos w
    //
    frm_splash.finish(&w);
    w.setWindowTitle(w.strTitulo);
    w.setWindowIcon(QIcon(":/imagenes/cargadora3.jpeg"));
    w.showMaximized();
    w.funInicio();
    w.show();


    return a.exec();
}
