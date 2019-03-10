#ifndef RANDOMGEN_H
#define RANDOMGEN_H

#include <QObject>
#include <QRandomGenerator>


class RandomGen : public QObject
{
    Q_OBJECT
public:
    explicit RandomGen(QObject *parent = nullptr);

signals:
    // void generated();

public slots:
    QString generate_text();

private:
    QRandomGenerator qrg;
};

#endif // RANDOMGEN_H
