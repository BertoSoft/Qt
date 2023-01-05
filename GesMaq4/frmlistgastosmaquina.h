#ifndef FRMLISTGASTOSMAQUINA_H
#define FRMLISTGASTOSMAQUINA_H

#include <QDialog>

namespace Ui {
class frmListGastosMaquina;
}

class frmListGastosMaquina : public QDialog
{
    Q_OBJECT

public:
    explicit frmListGastosMaquina(QWidget *parent = nullptr);
    ~frmListGastosMaquina();


    //
    // Variables Globales
    //
    QString GstrTitulo;
    int     Gfila_seleccionada = -1;

    void    funInicio(void);
    void    funRellenaListado(void);
    void    funProcesaFecha0();
    void    funProcesaFecha1();


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_tbFecha0_clicked();

    void on_tbFecha1_clicked();

    void on_txtFecha0_returnPressed();

    void on_txtFecha1_returnPressed();

    void on_cmbMaquina_activated(int index);

private:
    Ui::frmListGastosMaquina *ui;
};

#endif // FRMLISTGASTOSMAQUINA_H
