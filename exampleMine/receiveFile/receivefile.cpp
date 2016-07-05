#include "receivefile.h"
#include "ui_receivefile.h"

receiveFile::receiveFile(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::receiveFile),
    totalBytes(0),
    bytesReceived(0),
    fileNameSize(0)
{
    ui->setupUi(this);
    ui->progressBar->reset();

    connect(ui->BT_listen, SIGNAL(clicked(bool)), this, SLOT(startListen_clicked()));
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

receiveFile::~receiveFile()
{
    delete ui;
}

void receiveFile::start()
{
    ui->BT_listen->setEnabled(false);
    ui->progressBar->reset();
    this->resetVar();
    if(!tcpServer.listen(QHostAddress(QHostAddress::Any), 5566))
    {
        qDebug() << "tcpServer listen error:" << tcpServer.errorString();
        tcpServer.close();
        return;
    }
    ui->LB_showMsg->setText(tr("listening..."));
}

void receiveFile::acceptConnection()
{
    tcpServerConnection = new QTcpSocket;
    tcpServerConnection = tcpServer.nextPendingConnection();

    connect(tcpServerConnection, SIGNAL(readyRead()), this, SLOT(updateFile()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    tcpServer.close();
}

void receiveFile::updateFile()
{
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_5_7);

    if(bytesReceived <= sizeof(qint64)*2)
    {
        if( (tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0) )
        {
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;
        }

        if( (tcpServerConnection->bytesAvailable() >= fileNameSize) && (fileNameSize != 0) )
        {
            in >> fileName;
            qDebug() << "start recevice file :" << fileName;
            ui->LB_showMsg->setText(tr("receive file %1 ...").arg(fileName));
            bytesReceived += fileNameSize;
            localFile = new QFile(fileName);
            if(!localFile->open(QFile::WriteOnly))
            {
                qDebug() << "open file error!";
                return;
            }

        }
        else return;
    }
    if( bytesReceived < totalBytes )
    {
        bytesReceived += tcpServerConnection->bytesAvailable();
        inBlock = tcpServerConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesReceived);

    if( bytesReceived == totalBytes)
    {
        tcpServerConnection->close();
        localFile->close();
        ui->LB_showMsg->setText(tr("received file %1 success!").arg((fileName)));
        ui->BT_listen->setEnabled(true);

    }
}

void receiveFile::startListen_clicked()
{
    start();
}

void receiveFile::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "socket error:" << tcpServerConnection->errorString();
    tcpServerConnection->close();
    if(localFile->isOpen())
        localFile->close();
    ui->LB_showMsg->setText(tr("try again!"));
    ui->BT_listen->setEnabled(true);
    ui->progressBar->reset();

}

void receiveFile::resetVar()
{
    totalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    inBlock.resize(0);

}

