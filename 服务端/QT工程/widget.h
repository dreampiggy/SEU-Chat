#ifndef WIDGET_H
#define WIDGET_H
#include <QtNetwork>
#include <QWidget>

namespace Ui {
    class Widget;
}

enum MessageType{Message, NewParticipant, ParticipantLeft, FileName, Refuse,Xchat};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void openDataBase();
    void onlineUsr();
    void offlineUsr();
    void newParticipant(QString userName, QString localHostName, QString ipAddress);
    void participantLeft(QString userName, QString localHostName, QString time);
    int checkLogin();  //数据库操作检查登录函数
    bool  checkUsr(); //检查是否存在当前用户
    void  addUsr();//向数据库中增加用户

private:
    Ui::Widget *ui;
    qint32 port1;
    qint32 port2;
    qint32 xport;
    qint16 loginPort;
    //qint16 loginPortSend;
    qint16 logonPort;
    //qint16 logonPortSend;
    qint16 blocksize;
    qint16 blocksize2;
    QString logonUsrName;
    QString logonPwd;
    QString loginUsrName;
    QString loginPwd;
    QUdpSocket *receiver; //++
    QUdpSocket *send;
    QTcpServer *loginServer;
    QTcpServer *logonServer;
    QTcpSocket *loginConnection;
    QTcpSocket *logonConnection;


//++
private slots:
     void processPendingDatagrams();
     void acceptLogin();//接受登录
     void acceptLogon();//接受注册
     void checkUsrLogon(); //检查注册
     void checkUsrLogin(); //检查登录 返回-1,0,1;
};

#endif // WIDGET_H
