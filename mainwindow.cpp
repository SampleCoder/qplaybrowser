#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    times = 0;
    QObject::connect(this, SIGNAL(randomTextChanged(const QString &)), this, SLOT(on_label_setText()));

    ui->setupUi(this);

    setWindowTitle("Quick Applications Ltd.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked() {
    QApplication::quit();
}

void MainWindow::on_label_setText() {
    times += 1;
    QString playText{ QString::number(times) };

    playText += " hello(s)!";
    ui->label->setText(playText + " [" + rg.generate_text() + "]");
}

void MainWindow::on_pushButton_clicked() {

    emit randomTextChanged(rg.generate_text());

}

QStringList MainWindow::getDirectoryNames(const QString & dirName) {
    QStringList result;
    DIR *dirp;
    __off_t totalAmount = 0;
    unsigned int filesCount = 0;
    dirp = opendir(dirName.toStdString().c_str());
    if (dirp) {
        dirent *pDirEnt;
        while (true) {
            pDirEnt = readdir(dirp);
            if (!pDirEnt) break;
            QString entry { pDirEnt->d_name };

            entry += "\t";
            if (entry.size() < 16)
                entry += "\t";
            if (pDirEnt->d_type == DT_DIR)
                entry += "[DIR]";

            if (pDirEnt->d_type == DT_REG) {
                entry += "[FILE]";

                QString fullpath(dirName);
                fullpath.append("/") += pDirEnt->d_name;
                int fd = open(fullpath.toStdString().c_str(), O_RDONLY);
                if (fd != -1) {
                    struct stat fileStat;

                    filesCount += 1;
                    if (fstat(fd, &fileStat) == 0) {
                        entry += " " + (fileStat.st_size > 1024 ? QString::number(fileStat.st_size / 1024) + " KiB" :
                                                                  QString::number(fileStat.st_size) + " byte(s)");
                        totalAmount += fileStat.st_size;
                    }
                }
            }

            if (pDirEnt->d_type == DT_LNK)
                entry += "[LINK]";
            result.append(entry);
        }
        closedir(dirp);
        ui->label_Total->setText(QString("Total ") +
                                 QString::number(totalAmount > 1024 ? totalAmount / 1024 : totalAmount)
                                 + (totalAmount > 1024 ? " KiB in " : " byte(s) in ") +
                                 QString::number(filesCount) + " file(s)");
    }
    return result;
}

#include <QDebug>
void MainWindow::on_pushButton_3_clicked() {
    auto path = ui->lineEdit->text();
    if (!path.startsWith("/")) {
        path = currentPath + "/" + path;
    }
    if (path.length() == 0) return;
    auto dirNames = getDirectoryNames(path);
    if (dirNames.empty()) {
        if (errno != 0) {
            QMessageBox::critical(this, "Error: unable to get directory contents", strerror(errno));
        }
    } else {

        if (ui->listWidget->count() != 0) ui->listWidget->clear();

        ui->label->setText("Showing directory \"" + path + "\".");
        dirNames.sort();
        ui->listWidget->addItems(dirNames);

        for (int i = 0; i < ui->listWidget->count(); ++ i) {
            if (ui->listWidget->item(i)->text().contains("[DIR]")) {
                ui->listWidget->item(i)->setTextColor(Qt::blue);
            }
            if (ui->listWidget->item(i)->text().contains("[LINK]")) {
                ui->listWidget->item(i)->setTextColor(Qt::cyan);
            }
        }

        currentPath = path;
    }

}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{

}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {

    auto fullpath = currentPath;
    if (!fullpath.endsWith("/")) fullpath.append("/");
    auto filename = item->text().split("\t")[0];

    if (filename == "..") {
        auto stdFullPath = fullpath.toStdString();
        auto p1 = stdFullPath.size() - 1;
        while (p1 > 0) {
            if (stdFullPath[p1] == '/') p1 --; else break;
        }
        while (p1 > 0) {
            if (stdFullPath[p1] != '/') p1 --; else break;
        }
        fullpath = QString::fromStdString(stdFullPath.substr(0, p1 + 1));

    } else if (filename != ".") {
        fullpath += filename;
    }

    if (item->text().contains("[FILE]")) {
        // TODO: Search "open using executable" method.
        QProcess p;
        p.start("/bin/xdg-open", { fullpath });
        p.waitForFinished(0xFF000E3);
    } else if (item->text().contains("[DIR]")) {
        ui->lineEdit->setText(fullpath);
        on_pushButton_3_clicked();
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_3_clicked();
}
