#ifndef SENDWIDGET_H
#define SENDWIDGET_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>

namespace Ui {
class sendwidget;
}

class sendwidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit sendwidget(QWidget *parent = 0);
    ~sendwidget();

private:
    QTcpSocket *tcpClient;
    QFile *localFile;
    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 loadSize;
    QString fileName;
    QByteArray outBlock;

private slots:
    void send();
    void startTransfer();
    void updateClinetProgress(qint64);
    void openFile();
    void BT_openFile_clicked();
    void BT_sendFile_clicked();
    void displayError(QAbstractSocket::SocketError);


private:
    Ui::sendwidget *ui;
};

#endif // SENDWIDGET_H
