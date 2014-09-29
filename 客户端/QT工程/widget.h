#ifndef WIDGET_H
#define WIDGET_H

#include <QtNetwork>
#include <QWidget>
#include <QUdpSocket>
#include <QTextFormat>
#include <QtGui>
#include "tcpsend.h"
#include "tcpreceive.h"
#include "chat.h"

using namespace std;

namespace Ui {
    class Widget;
}


//enum MessageType{Message, NewParticipant, ParticipantLeft, FileName, Refuse,Xchat};//几种传送消息的状态

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
     chat* privatechat;
     chat* privatechat1;
protected:
    void changeEvent(QEvent *e);
    void initChatRoom(Widget*);
    void newParticipant(QString userName, QString localHostName, QString ipAddress); //新用户加入
    void participantLeft(QString userName, QString localHostName, QString time);  //用户离开
    void sendMessage(MessageType type, QString serverAddress=""); //向客户端广播消息

    void hasPendingFile(QString userName, QString serverAddress, QString clientAddress, QString fileName);
    //传送文件

    QString getIP(); //获取本机ip
    QString getUserName(); //获取本机用户名
    QString getMessage(); //获取消息

    void closeEvent(QCloseEvent *); //
    //void resizeEvent(QResizeEvent *event);
private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket; //广播消息的socket
    QUdpSocket *receiver; //接受消息的socket
    qint32 port1; //客户端发送消息到服务端,服务端接受消息的端口
    qint32 port2;  //服务端发送消息到客户端，客户端接受消息的端口
    qint32 xport;
    qint32 bb;
    TcpSend *server; //传送文件的客户端类
    QTcpSocket *tcpClient;  //传送文件的socket
    QString fileName; //传送文件名
    QColor color;
    //QImage bkImg;
    bool saveFile(const QString& filename);//保存聊天记录
    void showxchat(QString name,QString ip);
private slots:
    //new
   //void on_logoutButton_clicked();
    //void on_pushButton_clicked();
    void on_userTableWidget_doubleClicked(QModelIndex index);
    void on_save_clicked();

    void processPendingDatagrams(); //客户端处理接受的消息
    void getFileName(QString); //获得文件名
    void currentFormatChanged(const QTextCharFormat &format); //改变

    void on_sendButton_clicked();//发送消息
    void on_sendToolBtn_clicked(); //
    void on_fontComboBox_currentFontChanged(const QFont &f); //改变字体
    void on_sizeComboBox_currentIndexChanged(const QString &arg1); //改变字体大小
    void on_boldToolBtn_clicked(bool checked); //粗体
    void on_italicToolBtn_clicked(bool checked); //斜体
    void on_underlineToolBtn_clicked(bool checked); //加下划线
    void on_colorToolBtn_clicked();//字体颜色
    void on_clearToolBtn_clicked();//清除聊天消息列表
    void on_exitButton_clicked();//退出
};

#endif // WIDGET_H
