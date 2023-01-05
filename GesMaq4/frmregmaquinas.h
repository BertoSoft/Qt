#ifndef FRMREGMAQUINAS_H
#define FRMREGMAQUINAS_H

#include <QDialog>
#include <QListWidgetItem>


namespace Ui {
class frmRegMaquinas;
}

class frmRegMaquinas : public QDialog
{
    Q_OBJECT

public:
    explicit frmRegMaquinas(QWidget *parent = nullptr);
    ~frmRegMaquinas();

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

    void on_txtMarca_returnPressed();

    void on_txtModelo_returnPressed();

    void on_txtApodo_returnPressed();

    void on_txtBastidor_returnPressed();

    void on_txtHoras_returnPressed();

    void on_txtMatricula_returnPressed();

    void on_cmdGuardar_clicked();

    void on_tbEliminar_clicked();

    void on_tbFoto_clicked();

    void on_lstMaquinas_itemClicked(QListWidgetItem *item);

    void on_lstMaquinas_itemDoubleClicked(QListWidgetItem *item);

    void on_txtHoras_textChanged(const QString &arg1);

private:
    Ui::frmRegMaquinas *ui;
};

#endif // FRMREGMAQUINAS_H
