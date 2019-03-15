#ifndef FSINFO_H
#define FSINFO_H

#include <QObject>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


class FsInfo : public QObject
{
    Q_OBJECT
public:
    explicit FsInfo(const QString & directoryPath, QObject *parent = nullptr);
    ~FsInfo();
    bool open();
    int getErrorCode();
    int getFilesCount();
    long getFilesSize();
    const QString & getErrorString();
    const QStringList & getTextInfo();

    QString translateUnits(long bytes);

    static QString combinePath(const QStringList & components);

signals:

public slots:

private:
    QString processDirent(const struct dirent *dirEntry);

    QStringList textInfo;
    int errNo;
    int filesCount;
    long filesSize;
    QString errorString;
    QString dirPath;
    DIR * pDir;
};

#endif // FSINFO_H
