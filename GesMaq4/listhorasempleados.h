#ifndef LISTHORASEMPLEADOS_H
#define LISTHORASEMPLEADOS_H

#include <QDialog>

namespace Ui {
class listHorasEmpleados;
}

class listHorasEmpleados : public QDialog
{
    Q_OBJECT

public:
    explicit listHorasEmpleados(QWidget *parent = nullptr);
    ~listHorasEmpleados();

private slots:
    void on_cmdVolver_clicked();

private:
    Ui::listHorasEmpleados *ui;
};

#endif // LISTHORASEMPLEADOS_H
