#ifndef TCPSEND_H
#define TCPSEND_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTime>

namespace Ui {
     class TcpSend;
}

class TcpSend : public QDialog
{
    Q_OBJECT
    
public:
    explicit TcpSend(QWidget *parent = 0);
    ~TcpSend();
    void refused();

    void initServer();

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
    
private:
    Ui::TcpSend *ui;
    qint16 tcpPort;
    QTcpServer *tcpServer;
    QString fileName;
    QString theFileName;
    QFile *localFile;

    qint64 TotalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 playloadSize;
    QByteArray outBlock;

    QTcpSocket *clientConnection;
    QTime time;

private slots:
    void on_serverOpenBtn_clicked();
    void on_serverSendBtn_clicked();
    void on_serverCloseBtn_clicked();
    void sendMessage();

    void updateClientProgress(qint64 numBytes);

signals:
    void sendFileName(QString fileName);


};

#endif // TCPSEND_H
