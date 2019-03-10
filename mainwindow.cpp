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
    dirp = opendir(dirName.toStdString().c_str());
    if (dirp) {
        dirent *pDirEnt;
        while (true) {
            pDirEnt = readdir(dirp);
            if (!pDirEnt) break;
            QString entry { pDirEnt->d_name };

            entry += "\t\t";
            if (pDirEnt->d_type == DT_DIR)
                entry += "[DIR]";

            if (pDirEnt->d_type == DT_REG) {
                entry += "[FILE]";

                QString fullpath(dirName);
                fullpath.append("/") += pDirEnt->d_name;
                int fd = open(fullpath.toStdString().c_str(), O_RDONLY);
                if (fd != -1) {
                    struct stat fileStat;
                    if (fstat(fd, &fileStat) == 0) {
                        entry += " " + (fileStat.st_size > 1024 ? QString::number(fileStat.st_size / 1024) + " KiB" :
                                                                  QString::number(fileStat.st_size) + " byte(s)");
                    }
                }
            }

            if (pDirEnt->d_type == DT_LNK)
                entry += "[LINK]";
            result.append(entry);
        }
        closedir(dirp);
    }
    return result;
}

void MainWindow::on_pushButton_3_clicked() {
    // auto path = QInputDialog::getText(this, "Which dir?", "Input target directory name");
    auto path = ui->plainTextEdit->toPlainText();
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
        ui->listWidget->item(0)->setTextColor(Qt::green);
        ui->listWidget->repaint();

        currentPath = path;
    }

}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{

}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {

    auto fullpath = currentPath + "/" + item->text().split("\t")[0];
    if (item->text().contains("[FILE]")) {
        // TODO: Search "open using executable" method.
        auto filename = item->text().split("\t")[0];
        // execl("/bin/xdg-open", "/bin/xdg-open", (currentPath + "/" + filename).toStdString().c_str());
        QProcess p;
        p.start("/bin/xdg-open", { currentPath + "/" + filename });
        p.waitForFinished(0xFF000E3);
    }
    else if (item->text().contains("[DIR]")) {
        // TODO : Rework change directory model.
        ui->plainTextEdit->setPlainText(fullpath);
        on_pushButton_3_clicked();
    }
}
