#ifndef LOGON_H
#define LOGON_H
#include <QTcpSocket>
#include <QDialog>

namespace Ui {
    class logon;
}

class logon : public QDialog
{
    Q_OBJECT

public:
    explicit logon(QWidget *parent = 0);
    ~logon();

protected:
    void initLogon();
    void checkUsr();
    void newConnect();
    //QString getIp();

private:
    Ui::logon *ui;
    QTcpSocket *logonSocket;
    //QTcpSocket *logonReceive;

    QString usrName;
    QString pwd;
    qint16 logonPort;
    //qint16 receivePort;

private slots:
    void on_logonButton_clicked();
    void readLogonResult();
    void displayError(QAbstractSocket::SocketError);

/*signals:
    emit sendLogon();
    */
};

#endif // LOGON_H
