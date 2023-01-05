QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    frmcalendario.cpp \
    frmexportardatos.cpp \
    frmgesaverias.cpp \
    frmgesmantenimiento.cpp \
    frmgespartesdiarios.cpp \
    frmgesrepostajes.cpp \
    frmlistgastosmaquina.cpp \
    frmlisthorascentrotrabajo.cpp \
    frmlisthorasempleados.cpp \
    frmlisthorasmaquinas.cpp \
    frmlogin.cpp \
    frmlogin0.cpp \
    frmregcentrostrabajo.cpp \
    frmregempleados.cpp \
    frmregmantenimiento.cpp \
    frmregmaquinas.cpp \
    frmregprecioscombustible.cpp \
    frmregtarifasmaquinas.cpp \
    frmtareaspendientes.cpp \
    frmverdia.cpp \
    frmverpartes.cpp \
    funcaux.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    frmcalendario.h \
    frmexportardatos.h \
    frmgesaverias.h \
    frmgesmantenimiento.h \
    frmgespartesdiarios.h \
    frmgesrepostajes.h \
    frmlistgastosmaquina.h \
    frmlisthorascentrotrabajo.h \
    frmlisthorasempleados.h \
    frmlisthorasmaquinas.h \
    frmlogin.h \
    frmlogin0.h \
    frmregcentrostrabajo.h \
    frmregempleados.h \
    frmregmantenimiento.h \
    frmregmaquinas.h \
    frmregprecioscombustible.h \
    frmregtarifasmaquinas.h \
    frmtareaspendientes.h \
    frmverdia.h \
    frmverpartes.h \
    funcaux.h \
    mainwindow.h

FORMS += \
    frmcalendario.ui \
    frmexportardatos.ui \
    frmgesaverias.ui \
    frmgesmantenimiento.ui \
    frmgespartesdiarios.ui \
    frmgesrepostajes.ui \
    frmlistgastosmaquina.ui \
    frmlisthorascentrotrabajo.ui \
    frmlisthorasempleados.ui \
    frmlisthorasmaquinas.ui \
    frmlogin.ui \
    frmlogin0.ui \
    frmregcentrostrabajo.ui \
    frmregempleados.ui \
    frmregmantenimiento.ui \
    frmregmaquinas.ui \
    frmregprecioscombustible.ui \
    frmregtarifasmaquinas.ui \
    frmtareaspendientes.ui \
    frmverdia.ui \
    frmverpartes.ui \
    mainwindow.ui


#
# Añadido por mi para usar sqlite3
#
QT += sql

#
# Añadido por mi para usar Cryptopp
#
LIBS += -lcryptopp



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Imagenes.qrc
