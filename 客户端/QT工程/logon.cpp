#include "logon.h"
#include "ui_logon.h"
#include <QNetworkInterface>
#include <QMessageBox>
#include <QFile>

logon::logon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logon)
{
    ui->setupUi(this);
    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->logonButton->setStyleSheet("QPushButton:hover{color:rgb(140, 255, 188)}QPushButton:pressed{color:rgb(140, 255, 188)}");
    logonSocket=new QTcpSocket();
    logonPort=2413;
    //logonReceive=new QTcpSocket();
    connect(logonSocket,SIGNAL(readyRead()),this,SLOT(readLogonResult()));
    connect(logonSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                 this,SLOT(displayError(QAbstractSocket::SocketError)));

}

logon::~logon()
{
    delete ui;
}

void logon::on_logonButton_clicked()
{
    //checkPwd();
    usrName=ui->usrLineEdit->text().trimmed(); //移除开头和结尾的空白字符
    pwd=ui->pwdLineEdit->text().trimmed();
    if(usrName==""||pwd=="")  //用户名或密码不能为空
          QMessageBox::warning(0, tr("Error"),tr("用户名或密码不能为空"), QMessageBox::Ok);
    else
         newConnect();
}

//检测密码长度，是否含有非法字符
/*
void checkPwd()
{


}
*/

/*
QString logon::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
      foreach (QHostAddress address, list)
      {
          //使用IPv4地址
          if(address.protocol() == QAbstractSocket::IPv4Protocol)
          {
              if (address.toString().contains("127.0.")) //避免显示127.0.0.1
               {
                  continue;
               }
              else
              {
              return address.toString();
             }
        }
      }
    return 0;

}
*/
void logon::newConnect()
{
    QFile file(":/imgs/imgs/serverIP.txt");
    QString serverIPAddress;//从serverIP.txt获取IP,记住不要有空格之类
      if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          qDebug()<<"Can't open the file!"<<endl;
      }
      while(!file.atEnd()) {
          QByteArray line = file.readLine();
          qDebug()<<line;
          serverIPAddress=line;
      }
    logonSocket->abort();
    logonSocket->connectToHost(serverIPAddress,logonPort); //服务器地址，端口

    QByteArray usrData;
    QDataStream out(&usrData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint16) 0;
    qDebug()<<usrName << pwd;
    out <<usrName << pwd;
    out.device()->seek(0);
    qDebug() <<usrData.size();
    out<<(quint16)(usrData.size()-sizeof(quint16));
    logonSocket->write(usrData);
    qDebug()<<"newConect Logon";
}

void logon::readLogonResult()
{
    qDebug()<<"readLogon!" ;
     QDataStream in(logonSocket);
     QString result;
     in.setVersion(QDataStream::Qt_4_6);
     in >>result;
     qDebug ()<< result;
     if(result=="existUsr")
     {
         QMessageBox::warning(0,tr("Error"), tr("用户已存在"),QMessageBox::Ok);
         ui->usrLineEdit->clear();//清空用户名输入框
         ui->pwdLineEdit->clear();//清空密码输入框
         ui->usrLineEdit->setFocus();//将光标转到用户名输入框
     }
     if(result=="pass")
     {
         QMessageBox::information(0,tr("提示") ,tr("注册成功！请返回登录"),QMessageBox::Ok);
         ui->usrLineEdit->clear();//清空用户名输入框
         ui->pwdLineEdit->clear();//清空密码输入框
         ui->usrLineEdit->setFocus();//将光标转到用户名输入框
         logonSocket->abort();
         this->close();
     }
}
/*
void logon::checkUsr(QString usr,QString pwd)
{
    QString ipAddress = getIP();
    QByteArray usrdata;
    QDataStream out(&usrdata, QIODevice::WriteOnly);
    out << ipAddress << usr << pwd;
    logonSocket->write(usrdata);
}
*/
void logon::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
      {
          case QAbstractSocket::RemoteHostClosedError : break;
          default : qDebug() << logonSocket->errorString();
      }
}
