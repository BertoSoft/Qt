#ifndef FRMREGMANTENIMIENTO_H
#define FRMREGMANTENIMIENTO_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class frmRegMantenimiento;
}

class frmRegMantenimiento : public QDialog
{
    Q_OBJECT

public:
    explicit frmRegMantenimiento(QWidget *parent = nullptr);
    ~frmRegMantenimiento();

    //
    // Variables Globales
    //
    QString GstrRutaFoto = "";
    QString GstrTitulo;

    void funInicio(void);
    void funRellenaListado(void);
    void funLimpiaControles(void);
    void funHabilitaControles(void);
    void funDesHabilitaControles(void);


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_cmdGuardar_clicked();

    void on_tbAgregar_clicked();

    void on_tbEliminar_clicked();

    void on_tlMantenimiento_itemClicked(QTableWidgetItem *item);

    void on_tlMantenimiento_itemDoubleClicked(QTableWidgetItem *item);

    void on_txtNombre_returnPressed();

    void on_txtDescripcion_returnPressed();

    void on_txtLimiteHoras_returnPressed();

    void on_txtPrecio_returnPressed();

    void on_cmbPeriocidad_currentTextChanged(const QString &arg1);

    void on_cmbTipoMantenimiento_currentTextChanged(const QString &arg1);

    void on_txtLimiteHoras_textChanged(const QString &arg1);

    void on_txtPrecio_textChanged(const QString &arg1);

private:
    Ui::frmRegMantenimiento *ui;
};

#endif // FRMREGMANTENIMIENTO_H
