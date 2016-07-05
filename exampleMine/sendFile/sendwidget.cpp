#include "sendwidget.h"
#include "ui_sendwidget.h"
#include <QFileDialog>
#include <QHostAddress>

sendwidget::sendwidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sendwidget),
    totalBytes(0),
    bytesWritten(0),
    bytesToWrite(0),
    loadSize(4*1024)

{
    ui->setupUi(this);
    tcpClient = new QTcpSocket(this);
    connect(ui->BT_openFile, SIGNAL(clicked(bool)), this, SLOT(BT_openFile_clicked()));
    connect(ui->BT_sendFile, SIGNAL(clicked(bool)), this, SLOT(BT_sendFile_clicked()));

    connect(tcpClient, SIGNAL(connected()), this, SLOT(startTransfer()));
    connect(tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClinetProgress(qint64)));
    connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    ui->BT_sendFile->setEnabled(false);
    ui->progressBar->reset();

}

sendwidget::~sendwidget()
{
    delete ui;
}

void sendwidget::send()
{
    ui->BT_sendFile->setEnabled(false);
    bytesWritten = 0;

    ui->LB_status->setText(tr("connecting..."));
    tcpClient->connectToHost(QString("127.0.0.1"), 5566);

}

void sendwidget::startTransfer()
{
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug()<<"open file error";
        ui->LB_showMsg->setText(tr("open file error"));
        return;
    }
    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_7);
    QString currentFileName = fileName.right(fileName.size()-fileName.lastIndexOf('/') - 1);
    sendOut << qint64(0) << qint64(0) << currentFileName;
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64)*2));

    bytesToWrite = totalBytes - tcpClient->write(outBlock);
    ui->LB_status->setText(tr("connected"));
    outBlock.resize(0);
    qDebug() << "start transfer file";


}

void sendwidget::updateClinetProgress(qint64 numBytes)
{
//  qDebug()<< "## bytes has written:" << bytesWritten << "; bytes leave:" << bytesToWrite;
    bytesWritten += (int)numBytes;

    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));
        bytesToWrite -= (int)tcpClient->write(outBlock);

        outBlock.resize(0);
    } else {
        localFile->close();
    }

    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesWritten);

    if(bytesWritten == totalBytes)
    {
        ui->LB_status->setText(tr("send file %1 success").arg(fileName));
        if(localFile->isOpen()){
            localFile->close();
        }
        tcpClient->close();
    }
}

void sendwidget::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if( !fileName.isEmpty() )
    {
        ui->BT_sendFile->setEnabled(true);
        ui->LB_showMsg->setText(tr("open file %1 ok!").arg(fileName));
    }
    ui->progressBar->reset();
}


void sendwidget::BT_openFile_clicked()
{
    openFile();
}

void sendwidget::BT_sendFile_clicked()
{
    send();
}

void sendwidget::displayError(QAbstractSocket::SocketError)
{
    qDebug() << "tcpClient error:" << tcpClient->errorString();
    tcpClient->close();
    ui->progressBar->reset();
    ui->LB_status->setText(tr("error and reset"));
    ui->BT_sendFile->setEnabled(true);

}
