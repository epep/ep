#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QProgressDialog>
#include <QTimer>
#include "huffman.h"
#include "CkTar.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_CompBtn_clicked();

    void on_UncompBtn_clicked();

    void on_TarfileBtn_clicked();

    void on_UntarfileBtn_clicked();

private:

    Ui::MainWindow *ui;
    QProgressDialog *progDlg;
    QTimer *timer;
};
#endif // MAINWINDOW_H
