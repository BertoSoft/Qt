#ifndef FRMEXPORTARDATOS_H
#define FRMEXPORTARDATOS_H

#include <QDialog>

namespace Ui {
class frmExportarDatos;
}

class frmExportarDatos : public QDialog
{
    Q_OBJECT

public:
    explicit frmExportarDatos(QWidget *parent = nullptr);
    ~frmExportarDatos();

    //
    // Variables Globales
    //
    QString strTitulo;

    //
    // Funciones Publicas
    //
    void funInicio();

protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:

    void on_cmdVolver_clicked();

private:
    Ui::frmExportarDatos *ui;
};

#endif // FRMEXPORTARDATOS_H
