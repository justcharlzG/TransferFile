#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QMainWindow>
#include <QFile>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class receiveFile;
}

class receiveFile : public QMainWindow
{
    Q_OBJECT

public:
    explicit receiveFile(QWidget *parent = 0);
    ~receiveFile();

private:
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64 totalBytes;
    qint64 bytesReceived;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

private slots:
    void start();
    void acceptConnection();
    void updateFile();
    void startListen_clicked();
    void displayError(QAbstractSocket::SocketError socketError);
    void resetVar();


private:
    Ui::receiveFile *ui;
};

#endif // RECEIVEFILE_H
