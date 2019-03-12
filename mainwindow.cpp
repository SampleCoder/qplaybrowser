#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "fsinfo.h"


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
    FsInfo fsInfo(dirName);
    if (fsInfo.open()) {
        result = fsInfo.getTextInfo();
        ui->label_Total->setText(
            QString("Total ") +
                    fsInfo.translateUnits(fsInfo.getFilesSize()) +
                    " in " + QString::number(fsInfo.getFilesCount()) + " file(s)."
        );
    }
    return result;
}

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


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {

    auto fullpath = currentPath;
    if (!fullpath.endsWith("/")) fullpath.append("/");
    auto filename = item->text().split("\t")[0];

    if (filename == "..") {
        auto p1 = fullpath.size() - 1;
        while (p1 > 0) {
            if (fullpath[p1] == '/') p1 --; else break;
        }
        while (p1 > 0) {
            if (fullpath[p1] != '/') p1 --; else break;
        }
        fullpath = fullpath.left(p1 + 1);

    } else if (filename != ".") {
        fullpath += filename;
    }

    if (item->text().contains("[FILE]")) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullpath));
    } else if (item->text().contains("[DIR]")) {
        ui->lineEdit->setText(fullpath);
        on_pushButton_3_clicked();
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_3_clicked();
}
