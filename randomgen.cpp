#include "randomgen.h"


RandomGen::RandomGen(QObject *parent) : QObject(parent)
{
    qrg.seed();
}

QString RandomGen::generate_text()
{
    return QString::number(qrg.generate());
}

