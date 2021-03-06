#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_CompBtn_clicked()
{
    // 获取要压缩的文件名字
    QString filename = QFileDialog::getOpenFileName(this, tr("请选择要压缩的文件"));

    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        ui->InfilepathLabel->setText(filename);
    }

    // 获取要保存的文件名字
    QString savefilename = QFileDialog::getSaveFileName(this, tr("保存文件"));

    if (savefilename.isEmpty())
    {
        return;
    }
    else
    {
        // 开始压缩文件
        savefilename += ".huf";
        ui->OutfilepathLabel->setText(savefilename);
        Huffman hf(filename.toStdString(), savefilename.toStdString());
        hf.create_pq();
        hf.create_huffman_tree();
        hf.calculate_huffman_codes();
        hf.coding_save();
    }
}

void MainWindow::on_UncompBtn_clicked()
{
    // 获取要压缩的文件名字
    QString filename =
        QFileDialog::getOpenFileName(this, tr("请选择要解压的文件"), "", "*.huf");

    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        ui->InfilepathLabel->setText(filename);
    }

    // 获取要解压的文件名字
    QString savefilename = QFileDialog::getSaveFileName(this, tr("保存文件"));

    if (savefilename.isEmpty())
    {
        return;
    }
    else
    {
        // 开始解压文件
        ui->OutfilepathLabel->setText(savefilename);
        Huffman hf(filename.toStdString(), savefilename.toStdString());
        hf.recreate_huffman_tree();
        hf.decoding_save();
    }
}

void MainWindow::on_TarfileBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("请选择要打包的目录"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
    {
        return;
    }
    else
    {
        ui->InTarPathLabel->setText(dir);
    }


    // 开始压缩
    CkTar tar;

    bool success = tar.AddDirRoot(dir.toLatin1());

    if (success != true) {
        qDebug() << tar.lastErrorText();
        return;
    }

    // 获取要保存的文件名字
    QString savefilename = QFileDialog::getSaveFileName(this, tr("保存文件"));

    if (savefilename.isEmpty())
    {
        return;
    }
    else
    {
        ui->OutTarPathLabel->setText(savefilename);
        success = tar.WriteTar(savefilename.toLatin1());

        if (success != true) {
            qDebug() << tar.lastErrorText();
            return;
        }
    }
}

void MainWindow::on_UntarfileBtn_clicked()
{
    CkTar tar;

    // 获取要解包的文件名字
    QString filename =
        QFileDialog::getOpenFileName(this, tr("请选择要解包的文件"), "", "*.tar");

    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        ui->InTarPathLabel->setText(filename);
    }

    QString savedir = QFileDialog::getExistingDirectory(this,
                                                        tr("请选择要存放的目录"),
                                                        "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (savedir.isEmpty())
    {
        return;
    }
    else
    {
        ui->OutTarPathLabel->setText(savedir);
    }

    tar.put_UntarFromDir(savedir.toLatin1());
    tar.put_NoAbsolutePaths(true);


    // 开始解包，
    int fileCount = tar.Untar(filename.toLatin1());

    if (fileCount < 0) {
        qDebug() << tar.lastErrorText();
    }
    else {
        qDebug() << "Untarred " << fileCount << " files and directories";
    }
}
