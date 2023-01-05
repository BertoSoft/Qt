#ifndef FRMREGEMPLEADOS_H
#define FRMREGEMPLEADOS_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class frmRegEmpleados;
}

class frmRegEmpleados : public QDialog
{
    Q_OBJECT

public:
    explicit frmRegEmpleados(QWidget *parent = nullptr);
    ~frmRegEmpleados();

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

    void on_tbAgregar_clicked();

    void on_txtNombre_returnPressed();

    void on_txtApellidos_returnPressed();

    void on_txtDni_returnPressed();

    void on_txtDireccion_returnPressed();

    void on_txtTelefono_returnPressed();

    void on_txtTelefono_textChanged(const QString &arg1);

    void on_txtDni_textChanged(const QString &arg1);

    void on_cmdGuardar_clicked();

    void on_tbEliminar_clicked();

    void on_lstEmpleados_itemClicked(QListWidgetItem *item);

    void on_tbFoto_clicked();

    void on_lstEmpleados_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::frmRegEmpleados *ui;
};

#endif // FRMREGEMPLEADOS_H
