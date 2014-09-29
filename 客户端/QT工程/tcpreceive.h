#ifndef TCPRECEIVE_H
#define TCPRECEIVE_H

#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QTime>

namespace Ui {
    class TcpReceive;
}

class TcpReceive: public QDialog
{
    Q_OBJECT

public:
    explicit TcpReceive(QWidget *parent = 0);
    ~TcpReceive();
    void setHostAddress(QHostAddress address);
    void setFileName(QString fileName){localFile = new QFile(fileName);}
protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
private:
    Ui::TcpReceive *ui;
    QTcpSocket *tcpClient;
    quint16 blockSize;
    QHostAddress hostAddress;
    qint16 tcpPort;

    qint64 TotalBytes;
    qint64 bytesReceived;
    qint64 bytesToReceive;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

    QTime time;

private slots:
    void newConnect();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);

    void on_tcpClientCancelBtn_clicked();
    void on_tcpClientCloseBtn_clicked();
};

#endif // TCPRECEIVE_H
