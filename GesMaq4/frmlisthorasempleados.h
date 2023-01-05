#ifndef FRMLISTHORASEMPLEADOS_H
#define FRMLISTHORASEMPLEADOS_H

#include <QDialog>
#include <QTableWidget>


namespace Ui {
class frmListHorasEmpleados;
}

class frmListHorasEmpleados : public QDialog
{
    Q_OBJECT

public:
    explicit frmListHorasEmpleados(QWidget *parent = nullptr);
    ~frmListHorasEmpleados();


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

    void on_cmbEmpleado_activated(int index);

    void on_txtFecha0_returnPressed();

    void on_txtFecha1_returnPressed();

    void on_cmdVer_clicked();

    void on_tlListado_itemDoubleClicked(QTableWidgetItem *item);

    void on_tlListado_itemClicked(QTableWidgetItem *item);

private:
    Ui::frmListHorasEmpleados *ui;
};

#endif // FRMLISTHORASEMPLEADOS_H
