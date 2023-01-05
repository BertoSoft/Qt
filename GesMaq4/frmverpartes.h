#ifndef FRMVERPARTES_H
#define FRMVERPARTES_H

#include <QDialog>

namespace Ui {
class frmVerPartes;
}

class frmVerPartes : public QDialog
{
    Q_OBJECT

public:
    explicit frmVerPartes(QWidget *parent = nullptr);
    ~frmVerPartes();

    QString strTitulo;
    QString strFecha;
    QString strCodEmpleado;

    void funInicio();


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

private:
    Ui::frmVerPartes *ui;
};

#endif // FRMVERPARTES_H
