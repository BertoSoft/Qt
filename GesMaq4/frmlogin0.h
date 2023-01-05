#ifndef FRMLOGIN0_H
#define FRMLOGIN0_H

#include <QDialog>

namespace Ui {
class frmLogin0;
}

class frmLogin0 : public QDialog
{
    Q_OBJECT

public:
    explicit frmLogin0(QWidget *parent = nullptr);
    ~frmLogin0();

    //
    // Variables globales
    //
    QString strTitulo;
    QString GstrUsuario;
    QString GstrHoraInicio, GstrFechaInicio;


protected:

    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_cmdSalir_clicked();

    void on_cmdEntrar_clicked();

    void on_txtUsuario_returnPressed();

    void on_txtPasswd_returnPressed();

    void on_txtPasswd1_returnPressed();

private:
    Ui::frmLogin0 *ui;
};

#endif // FRMLOGIN0_H
