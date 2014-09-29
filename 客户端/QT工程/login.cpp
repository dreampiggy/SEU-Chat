#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include "loginName.h"
#include <QFile>
login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    initLogin(); //初始化登陆界面
    ui->pwdEdit->setEchoMode(QLineEdit::Password);
    usrlogon = new logon(this);
    //chatRoom = new Widget(this);
    loginSocket = new QTcpSocket(this);
     loginPort=3142;
   /* loginReceive= new QTcpSocket(this);
    loginPort=3142;
    receivePort=1234;
    loginReceive->connectToHost(QHostAddress::LocalHost,receivePort);
    connect(loginReceive,SIGNAL(readyRead()),this,SLOT(readLoginResult()));
    connect(loginReceive,SIGNAL(error(QAbstractSocket::SocketError)),
             this,SLOT(displayError(QAbstractSocket::SocketError)));
             */
    connect(loginSocket,SIGNAL(readyRead()),this,SLOT(readLoginResult()));
    connect(loginSocket,SIGNAL(error(QAbstractSocket::SocketError)),
             this,SLOT(displayError(QAbstractSocket::SocketError)));
}
void login::initLogin()
{
    ui->loginButton->setStyleSheet(tr("QPushButton:hover{color:rgb(140, 255, 188) }QPushButton:pressed{color:rgb(140, 255, 188)}"));
    ui->logonButton->setStyleSheet(tr("QPushButton:hover{color:rgb(140, 255, 188)} QPushButton:pressed{color:rgb(140, 255, 188)}"));

}
login::~login()
{
    delete ui;
}

void login::on_logonButton_clicked()
{
    usrlogon->show();  ///
}
QString login::getUsrName()
{
    return usrName;
}
void login::newConnect()
{
    //qDebug()<<"newConect";  ///////
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
    loginSocket->abort();
    loginSocket->connectToHost(serverIPAddress,loginPort); //服务器地址，端口
    /*
    QByteArray x;
    x.resize(2);
    x[0]=
            */
    QByteArray usrInfo;
    QDataStream out(&usrInfo, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << (quint16) 0;
    qDebug()<<usrName << pwd;
    out << usrName << pwd ;
    out.device()->seek(0);
    qDebug() << usrInfo.size();
    out<<(quint16)(usrInfo.size()-sizeof(quint16));
    //qDebug() << QVariant(usrInfo[1]).toInt()<<QVariant(usrInfo[2]).toInt();
    loginSocket->write(usrInfo);
    qDebug()<<"newConect";  //debug1

}

void login::on_loginButton_clicked()
{
    /*
    if(ui->pwdEdit->text()=="731720")
        accept();
    else
        QMessageBox::warning(this,tr("Warning"),tr("Password error!"),QMessageBox::Yes);
    */
    usrName=ui->usrEdit->text().trimmed();
    pwd=ui->pwdEdit->text().trimmed();
    if(usrName==""||pwd=="")
          QMessageBox::warning(0, tr("Error"),tr("用户名或密码不能为空"), QMessageBox::Ok);
    else
        newConnect();
}

void login::readLoginResult()
{
    qDebug()<<"readLogin!" ;  //debug2
    QDataStream in(loginSocket);
    QString result;
    in.setVersion(QDataStream::Qt_4_6);
    in >>result;
    qDebug ()<< result; //debug3

    if(result=="pass")  //当前窗口关闭，进入聊天室
    {

        //this->close();
       //chatRoom->show();
        loginName=usrName;
        accept();
        loginSocket->abort();
    }
    else
    {
    if(result=="noUsr")
    {
        QMessageBox::warning(0, tr("Error"),tr("用户不存在"), QMessageBox::Ok);
        ui->usrEdit->clear();//清空用户名输入框
        ui->pwdEdit->clear();//清空密码输入框
        ui->usrEdit->setFocus();//将光标转到用户名输入框
    }
    if(result=="wrongPwd")
    {
        QMessageBox::warning(0,  tr("Error"),tr("密码错误"), QMessageBox::Ok);
        ui->usrEdit->clear();//清空用户名输入框
        ui->pwdEdit->clear();//清空密码输入框
        ui->usrEdit->setFocus();//将光标转到用户名输入框
    }
    if(result=="haveLogin")
    {
        QMessageBox::warning(0,  tr("Error"),tr("用户已登陆"), QMessageBox::Ok);
        ui->usrEdit->clear();//清空用户名输入框
        ui->pwdEdit->clear();//清空密码输入框
        ui->usrEdit->setFocus();//将光标转到用户名输入框
    }
}
}

void login::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
      {
          case QAbstractSocket::RemoteHostClosedError : break;
          default : qDebug() << loginSocket->errorString();
      }
}

