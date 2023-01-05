#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QDialog>

namespace Ui {
class frmLogin;
}

class frmLogin : public QDialog
{
    Q_OBJECT

public:
    explicit frmLogin(QWidget *parent = nullptr);
    ~frmLogin();

    //
    // Variables globales
    //
    QString strTitulo;
    QString GstrUsuario;
    QString GstrHoraInicio, GstrFechaInicio;


protected:

    bool eventFilter(QObject *obj, QEvent *event);


private slots:
    void on_txtUsuario_returnPressed();

    void on_txtPasswd_returnPressed();

    void on_cmdSalir_clicked();

    void on_cmdEntrar_clicked();

private:
    Ui::frmLogin *ui;
};

#endif // FRMLOGIN_H
