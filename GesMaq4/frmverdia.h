#ifndef FRMVERDIA_H
#define FRMVERDIA_H

#include <QDialog>

namespace Ui {
class frmVerDia;
}

class frmVerDia : public QDialog
{
    Q_OBJECT

public:
    explicit frmVerDia(QWidget *parent = nullptr);
    ~frmVerDia();


    QString strFecha;
    QString strCodEmpresa;

    void    funInicio();


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

private:
    Ui::frmVerDia *ui;
};

#endif // FRMVERDIA_H
