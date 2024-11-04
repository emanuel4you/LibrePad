// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#include "librepad.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Librepad w;
    w.show();
    return a.exec();
}
