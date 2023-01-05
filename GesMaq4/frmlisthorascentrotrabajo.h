#ifndef FRMLISTHORASCENTROTRABAJO_H
#define FRMLISTHORASCENTROTRABAJO_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class frmListHorasCentroTrabajo;
}

class frmListHorasCentroTrabajo : public QDialog
{
    Q_OBJECT

public:
    explicit frmListHorasCentroTrabajo(QWidget *parent = nullptr);
    ~frmListHorasCentroTrabajo();


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

    void on_cmbEmpresa_activated(int index);

    void on_cmdVer_clicked();

    void on_tlListado_itemClicked(QTableWidgetItem *item);

    void on_tlListado_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::frmListHorasCentroTrabajo *ui;
};

#endif // FRMLISTHORASCENTROTRABAJO_H
