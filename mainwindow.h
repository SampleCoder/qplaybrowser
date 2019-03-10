#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "randomgen.h"


#include <QMainWindow>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QProcess>
#include <QPlainTextEdit>


#include <string.h>
#include <dirent.h>


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

signals:
    void randomTextChanged(const QString & randomText);

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_label_setText();

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
  QStringList getDirectoryNames(const QString & dirName);
  QString currentPath;
  Ui::MainWindow *ui;
  RandomGen rg;
  int times;
};

#endif // MAINWINDOW_H
