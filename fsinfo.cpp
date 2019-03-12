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

QString FsInfo::translateUnits(long bytes) {
    if (bytes > 1024 * 1024) {
        return QString::number(bytes / (1024 * 1024)) + " MiB";
    }
    if (bytes > 1024) {
        return QString::number(bytes / 1024) + " KiB";
    }

    return QString::number(bytes) + " byte(s)";
}

QString FsInfo::combinePath(const QStringList & components) {

    if (components.isEmpty())
        return "";

    QString resultPath{ "" };

    for (int i = 0; i < components.size() - 1; ++ i) {
        resultPath += components[i];
        if (!resultPath.endsWith("/")) {
            resultPath += "/";
        }
    }

    resultPath += components.last();;
    return resultPath;
}

QString FsInfo::processDirent(const struct dirent *dirEntry) {
    QString entry{dirEntry->d_name};

    if (dirEntry->d_type == DT_DIR) {
        entry += "\t\t[DIR]";
    } else

    if (dirEntry->d_type == DT_BLK) {
        entry += "\t\t[BLK]";
    } else

    if (dirEntry->d_type == DT_CHR) {
        entry += "\t\t[CHR]";
    } else

    if (dirEntry->d_type == DT_FIFO) {
        entry += "\t\t[FIFO]";
    } else

    if (dirEntry->d_type == DT_SOCK) {
        entry += "\t\t[SOCK]";
    } else

    if (dirEntry->d_type == DT_LNK) {
        entry += "\t\t[LINK]";
        char linkPath[PATH_MAX];
        auto linkPathSize = readlink(
                    FsInfo::combinePath({ dirPath, dirEntry->d_name }).toStdString().c_str(),
                    linkPath, sizeof(linkPath) - 1);
        if (linkPathSize != -1) {
            linkPath[linkPathSize] = '\0';
            entry += " -> ";
            entry += linkPath;
        }
    } else

    if (dirEntry->d_type == DT_REG) {
        entry += "\t\t[FILE]";

        struct stat fStat;
        if (stat(
            FsInfo::combinePath({ dirPath, dirEntry->d_name }).toStdString().c_str(),
            &fStat) != -1) {

            filesSize += fStat.st_size;
            entry.append(" ") += translateUnits(fStat.st_size);

        } else {
            entry += " Stat failed: (";
            entry += strerror(errno);
            entry += " \"" + FsInfo::combinePath({ dirPath, dirEntry->d_name }) + "\")";
        }

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
