#ifndef FRMREGCENTROSTRABAJO_H
#define FRMREGCENTROSTRABAJO_H

#include <QDialog>
#include <QTableWidgetItem>


namespace Ui {
class frmregCentrosTrabajo;
}

class frmregCentrosTrabajo : public QDialog
{
    Q_OBJECT

public:
    explicit frmregCentrosTrabajo(QWidget *parent = nullptr);
    ~frmregCentrosTrabajo();

    //
    // Variables Globales
    //
    QString GstrTitulo;

    void funInicio(void);
    void funLimpiaControles();
    void funHabilitaControles();
    void funDesHabilitaControles();
    void funRellenaListado();



protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_tbAgregar_clicked();

    void on_txtEmpresa_returnPressed();

    void on_txtCif_returnPressed();

    void on_txtCentroTrabajo_returnPressed();

    void on_txtRazonSocial_returnPressed();

    void on_txtTelefono_returnPressed();

    void on_txtCodigo_returnPressed();

    void on_cmdGuardar_clicked();

    void on_tbEliminar_clicked();

    void on_tlCentrosTrabajo_itemClicked(QTableWidgetItem *item);

    void on_tlCentrosTrabajo_itemDoubleClicked(QTableWidgetItem *item);

    void on_txtCif_textChanged(const QString &arg1);

    void on_txtTelefono_textChanged(const QString &arg1);

private:
    Ui::frmregCentrosTrabajo *ui;
};

#endif // FRMREGCENTROSTRABAJO_H
