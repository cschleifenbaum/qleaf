######################################################################
# Automatically generated by qmake (3.1) Thu Aug 4 13:39:57 2022
######################################################################

TEMPLATE = app
TARGET = leaf
INCLUDEPATH += .

QT += serialbus
QT -= gui

LIBS += -L/usr/local/Qt-5.15.5/lib -lnymea-gpio

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += openinverter Stm32-vcu/include

# Input
HEADERS += tccharger.h canbusnode.h canbusnodedetector.h leafobcharger.h leafhvbattery.h canmessageutils.h leafchademoport.h lim.h Stm32-vcu/include/i3LIM.h openinverter/params.h chargingrelaycontroller.h tcchargercontroller.h
SOURCES += tccharger.cpp canbusnode.cpp main.cpp canbusnodedetector.cpp leafobcharger.cpp leafhvbattery.cpp canmessageutils.cpp leafchademoport.cpp lim.cpp Stm32-vcu/src/i3LIM.cpp openinverter/params.cpp chargingrelaycontroller.cpp tcchargercontroller.cpp
