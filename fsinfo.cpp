#include "fsinfo.h"

FsInfo::FsInfo(const QString & directoryPath, QObject *parent) :
    QObject(parent), dirPath(directoryPath), pDir(nullptr)
{

}

bool FsInfo::open() {
    pDir = opendir(dirPath.toStdString().c_str());

    return pDir != nullptr;
}

int FsInfo::getErrorCode() {
    errNo = errno;
    return errno;
}

const QString & FsInfo::getErrorString() {
    errorString = QString::fromStdString(strerror(errNo));
    return errorString;
}

QString FsInfo::processDirent(const struct dirent *dirEntry) {
    QString entry{dirEntry->d_name};

    if (dirEntry->d_type == DT_DIR) {
        entry += "\t\t[DIR]";
    }

    if (dirEntry->d_type == DT_REG) {
        entry += "\t\t[FILE]";

        int fd = ::open((dirPath + "/" + dirEntry->d_name).toStdString().c_str(), O_RDONLY);
        if (fd != -1) {
            struct stat fStat;
            if (fstat(fd, &fStat) != -1) {
                filesSize += fStat.st_size;
                entry += " ";
                if (fStat.st_size > 1024) {
                    entry += QString::number(fStat.st_size / 1024);
                    entry += " KiB";
                } else {
                    entry += QString::number(fStat.st_size);
                    entry += " byte(s)";
                }
            } else {
                entry += " Stat failed: (";
                entry += strerror(errno);
                entry += ")";
            }
        } else {
            entry += " Cannot open file: (";
            entry += strerror(errno);
            entry += ")";
        }

        ::close(fd);
    }
    return entry;
}

const QStringList & FsInfo::getTextInfo() {
    textInfo.clear();
    filesCount = 0;
    filesSize = 0;
    if (pDir) {
        struct dirent *pDirEnt;
        do {
            pDirEnt = readdir(pDir);
            if (!pDirEnt) break;
            filesCount += 1;
            textInfo.append( processDirent(pDirEnt) );
        } while (true);
    }
    textInfo.sort();
    return textInfo;
}

FsInfo::~FsInfo() {
    if (pDir) {
        closedir(pDir);
    }
}

int FsInfo::getFilesSize() {
    return filesSize;
}

int FsInfo::getFilesCount() {
    return filesCount;
}
