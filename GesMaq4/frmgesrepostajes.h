#ifndef FRMGESREPOSTAJES_H
#define FRMGESREPOSTAJES_H

#include <QDialog>
#include <QTableWidgetItem>


namespace Ui {
class frmGesRepostajes;
}

class frmGesRepostajes : public QDialog
{
    Q_OBJECT

public:
    explicit frmGesRepostajes(QWidget *parent = nullptr);
    ~frmGesRepostajes();

    //
    // Variables Globales
    //
    QString GstrTitulo;

    void funInicio(void);
    void funRellenaListado(void);
    void funProcesaFecha();


protected:

    bool eventFilter(QObject *obj, QEvent *event);


private slots:
    void on_cmdVolver_clicked();

    void on_tbFecha_clicked();

    void on_txtFecha_returnPressed();

    void on_cmbEmpresas_activated(int index);

    void on_cmbCombustible_activated(int index);

    void on_cmbEmpleados_activated(int index);

    void on_cmbMaquinas_activated(int index);

    void on_txtCantidad_textChanged(const QString &arg1);

    void on_tbAgregar_clicked();

    void on_cmdGuardar_clicked();

    void on_txtCantidad_returnPressed();

    void on_tlRepostajes_itemClicked(QTableWidgetItem *item);

    void on_tlRepostajes_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbEliminar_clicked();

private:
    Ui::frmGesRepostajes *ui;
};

#endif // FRMGESREPOSTAJES_H
