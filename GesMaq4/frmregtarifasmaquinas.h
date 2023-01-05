#ifndef FRMREGTARIFASMAQUINAS_H
#define FRMREGTARIFASMAQUINAS_H

#include <QDialog>
#include <QTableWidget>


namespace Ui {
class frmRegTarifasMaquinas;
}

class frmRegTarifasMaquinas : public QDialog
{
    Q_OBJECT

public:
    explicit frmRegTarifasMaquinas(QWidget *parent = nullptr);
    ~frmRegTarifasMaquinas();


    //
    // Variables Globales
    //
    QString GstrRutaFoto = "";
    QString GstrTitulo;
    int     fila_seleccionada = -1;

    void    funInicio(void);
    void    funRellenaListado(void);
    void    funLimpiaControles(void);
    void    funHabilitaControles(void);
    void    funDesHabilitaControles(void);

protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_tbAgregar_clicked();

    void on_cmdGuardar_clicked();

    void on_cmbCentroTrabajo_activated(int index);

    void on_cmbMaquina_activated(int index);

    void on_txtPrecio_returnPressed();

    void on_txtPrecio_textChanged(const QString &arg1);

    void on_tlTarifas_itemClicked(QTableWidgetItem *item);

    void on_tlTarifas_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbEliminar_clicked();

private:
    Ui::frmRegTarifasMaquinas *ui;
};

#endif // FRMREGTARIFASMAQUINAS_H
