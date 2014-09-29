#ifndef LOGIN_H
#define LOGIN_H
#include <QDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include "logon.h"

namespace Ui {
    class login;
}


class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    QString getUsrName();
    ~login();

protected:
    void initLogin( );
    void newConnect();

private:
    Ui::login *ui;
    QString usrName;
    QString pwd;
    QTcpSocket *loginSocket;
    //QTcpSocket *loginReceive;
    qint16 loginPort;
    //qint16 receivePort;
    logon *usrlogon;
    //Widget *chatRoom;

private slots:
    void readLoginResult();
    void displayError(QAbstractSocket::SocketError);
    void on_loginButton_clicked();
    void on_logonButton_clicked();
};

#endif // LOGIN_H
