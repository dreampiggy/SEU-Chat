#include "widget.h"
#include "ui_widget.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    openDataBase();
    loginPort=3142;
    //验证用户登录
    loginServer=new QTcpServer(this);
    if(!loginServer->listen(QHostAddress::Any,loginPort))
    {  //监听
        qDebug() << loginServer->errorString();
        close();
    }
    connect(loginServer,SIGNAL(newConnection()),this,SLOT(acceptLogin()));

    logonPort=2413;
    //验证用户注册
    logonServer= new QTcpServer(this);
    if(!logonServer->listen(QHostAddress::Any,logonPort))
    {  //监听
        qDebug() << logonServer->errorString();
        close();
    }
    qDebug() << "checkLogon";
    connect(logonServer,SIGNAL(newConnection()),this,SLOT(acceptLogon()));

    port1=6919;
    port2=8919;
    //xport=7777;
    receiver = new QUdpSocket(this);
    send=new QUdpSocket(this);
    qDebug() << "UDP start";
    receiver->bind(port1,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    send->bind(port2,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(receiver,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::processPendingDatagrams()
{
    while(receiver->hasPendingDatagrams())  //至少有一个数据包正被读取
    {
        QByteArray datagram;
        QByteArray data;
        datagram.resize(receiver->pendingDatagramSize());
        data.resize(send->pendingDatagramSize());

        receiver->readDatagram(datagram.data(), datagram.size());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        QDataStream outUDP(&data, QIODevice::WriteOnly);
        int messageType;
        in >> messageType;
        QString userName, localHostName, ipAddress, message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        switch(messageType)
        {

        case Message:
            in >> userName >> localHostName >> ipAddress >> message;
            outUDP <<messageType << userName << localHostName << ipAddress << message;
            //加入聊天记录保存
            break;

         //聊天历史记录读取
          /*case offLineMessage:


         */
        case NewParticipant:
            qDebug() << "server New has";
            in >> userName >> localHostName >> ipAddress;
            outUDP << messageType << userName << localHostName << ipAddress;
            newParticipant(userName, localHostName, ipAddress);
            break;

        case ParticipantLeft:
            qDebug()<<"server left has";
            in >> userName >> localHostName;
            outUDP<<messageType << userName << localHostName ;
            participantLeft(userName, localHostName, time);
            break;

        case FileName:
        {
            qDebug()<<"server filename has";
            in >> userName >> localHostName >> ipAddress;
            QString clientAddress, fileName;
            in >> clientAddress >> fileName;
            outUDP << messageType << userName << localHostName << ipAddress <<clientAddress << fileName;
            break;
        }

        case Refuse:
        {
            qDebug()<<"server refuse has";
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            outUDP << messageType << userName << localHostName << serverAddress;
            break;
        }

        case Xchat:
        {
          qDebug()<<"server Xchat has";
          in >>userName >>localHostName;
          QString serverAddress;
          in >> serverAddress;
          outUDP << messageType << userName << localHostName << serverAddress;
          break;
        }

        //如果没有注册，传给个人的离线文件就无意义，所以离线文件保存在服务端，公告给所有人。
        /*
         case offlineFilename:
        {
            in >> userName >> localHostName >> ipAddress ;
            QString clientAddress, fileName;
            in >> clientAddress >> fileName;
            handOfflineFile();
            out << messageType << userName << localHostName << ipAddress <<clientAddress << fileName;
            break;
        }
        */
        }
        send->writeDatagram(data, data.length(), QHostAddress::Broadcast, port2);
    }
}

void Widget::openDataBase()
{
           QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); //添加QMYSQL数据库驱动
            db.setHostName("localhost"); //设置登陆的主机名为localhost
            db.setUserName("root"); //登陆的用户是超级用户 root
            db.setPassword("123"); //登陆密码是123
            db.setDatabaseName("dolorsUsr"); //打开数据库
            bool ok = db.open();
    //布尔类型，打开数据。
            if(ok) //if判断
            {
                    qDebug()<<"success open DB!"<<endl;
    //如果打开数据库在终端输出open Database信息

            }
            else
            {
                   qDebug()<<"failed to open DB!"<<endl;
       //否则输出数据库的消息框错误信息
            }

}


void Widget::onlineUsr()
{
    QSqlQuery query;
    query.prepare("UPDATE usr SET status='online'  WHERE name=:name");
    query.bindValue(":name", loginUsrName);
    query.exec();

}

void Widget::offlineUsr()
{
    QSqlQuery query;
    query.prepare("UPDATE usr SET status='offline'  WHERE name=:name");
    query.bindValue(":name",loginUsrName);
    query.exec();
}

void Widget::newParticipant(QString userName, QString localHostName, QString ipAddress)
{
    bool isEmpty = ui->onlineUsrDB->findItems(userName, Qt::MatchExactly).isEmpty();

    if(isEmpty)
    {
        int myRow=ui->onlineUsrDB->rowCount();
        QTableWidgetItem* user = new QTableWidgetItem(userName);
        QTableWidgetItem* host = new QTableWidgetItem(localHostName);
        QTableWidgetItem* ip = new QTableWidgetItem(ipAddress);

        ui->onlineUsrDB->insertRow(myRow);
        ui->onlineUsrDB->setItem(myRow, 0, user);
        ui->onlineUsrDB->setItem(myRow, 1, host);
        ui->onlineUsrDB->setItem(myRow, 2, ip);

        ui->statusBrowser->setTextColor(Qt::green);
        ui->statusBrowser->setCurrentFont(QFont("Times New Roman", 12));
        ui->statusBrowser->append(tr(" %1 在线!").arg(userName));
        ui->userNumLabel->setText(tr("在线人数 : %1 ").arg(ui->onlineUsrDB->rowCount()));
    }
}

void Widget::participantLeft(QString userName, QString localHostName, QString time)
{
    int rowNum = ui->onlineUsrDB->findItems(userName, Qt::MatchExactly).first()->row();

    ui->onlineUsrDB->removeRow(rowNum);
    ui->statusBrowser->setTextColor(Qt::red);
    ui->statusBrowser->setCurrentFont(QFont("Times New Roman", 12));

    ui->statusBrowser->append(tr(" %1 于 %2 离开!").arg(userName).arg(time));
    ui->userNumLabel->setText(tr("在线人数 : %1 ").arg(ui->onlineUsrDB->rowCount()));
    offlineUsr();
}

void Widget::acceptLogin()
{
    qDebug() << "acceptLogin";  ///debug1
    loginConnection = loginServer->nextPendingConnection();
    connect(loginConnection,SIGNAL(readyRead()),this,SLOT(checkUsrLogin()));
    //connect(loginConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
}

void Widget::acceptLogon()
{
    /*
    if(!logonServer->listen(QHostAddress::Any, logonPort))
       {
           qDebug() << logonServer->errorString();
           close();
           return;
       }
       */
     qDebug() << "acceptLogon";
    logonConnection = logonServer->nextPendingConnection();
    connect(logonConnection,SIGNAL(readyRead()),this,SLOT(checkUsrLogon()));
    //connect(logonConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
}

void Widget::checkUsrLogin()
{
           blocksize=0;
           qDebug() << "checkLogin"; //debug2
           //QByteArray loginData;
          // loginData.resize(loginConnection->pendingDatagramSize());  //?

          // loginConnection->readDatagram(loginData.data(), loginData.size());

           QDataStream in(loginConnection);//读取用户注册的用户名
           in.setVersion(QDataStream::Qt_4_6);
           if(blocksize==0) //如果是刚开始接收数据
           {
                 if(loginConnection->bytesAvailable() < (int)sizeof(quint16))
                     return;
                 in >> blocksize;
                 qDebug()<< blocksize;
           }
           if(loginConnection->bytesAvailable() < blocksize)
                 return;
           in>>loginUsrName>>loginPwd;
           qDebug()<< loginUsrName << loginPwd;    //debug3
           QByteArray loginR;
           QDataStream out(&loginR, QIODevice::WriteOnly);
           out.setVersion(QDataStream::Qt_4_6);
           QString loginResult;
           switch(checkLogin())//判断用户登录有效性，成功返回1,密码错误返回0,用户不存在返回-1
           {
           case 2:
               loginResult= "pass";
               break;
           case 1:
               loginResult= "haveLogin";
               break;
           case 0:
               loginResult= "wrongPwd";
               break;
           case -1:
               loginResult= "noUsr";
               break;
           }
           out << loginResult;
          loginConnection->write(loginR); //发送注册结果
}

int Widget::checkLogin()
{

     QSqlQuery query;
     query.exec("select * from usr ORDER BY id ASC");
     int flag=-1;
     while(query.next())
     {
         if (loginUsrName==query.value(1).toString())
         {
             flag=0;
             if (query.value(2).toString()==loginPwd)
             {
                 flag=1;
                 if(query.value(3).toString()=="offline")
                 {
                     flag=2;
                     onlineUsr();
                     break;
                 }
             }
         }
     }
     return flag;
}

void Widget::checkUsrLogon()
{
           //usrData.resize(logonConnection->pendingDatagramSize());
           blocksize2=0;
           //logonConnection->readDatagram(usrData.data(), usrData.size());
           qDebug() << "checkLogon";
           QDataStream inLogon(logonConnection);//读取用户注册的用户名
           inLogon.setVersion(QDataStream::Qt_4_6);
           if(blocksize2==0) //如果是刚开始接收数据
                    {
                          if(logonConnection->bytesAvailable() < (int)sizeof(quint16))
                              return;
                          inLogon >> blocksize2;
                          qDebug()<< blocksize2;
                    }
                    if(logonConnection->bytesAvailable() < blocksize2)
                          return;
           inLogon>>logonUsrName>>logonPwd;
           qDebug() <<logonUsrName << logonPwd;

           ///////是否要加另一个port?
           QByteArray logonR;
           QDataStream out(&logonR, QIODevice::WriteOnly);
           out.setVersion(QDataStream::Qt_4_6);
           QString logonResult;
           if(checkUsr())//判断用户名是否存在
           {
               logonResult= "pass";
               addUsr();
           }
           else
               logonResult="existUsr";
            out << logonResult;
            logonConnection->write(logonR); //发送注册结果
}

void Widget::addUsr()
{
    qDebug() << "addUsr";
    QSqlQuery query;
    query.prepare("INSERT INTO usr (name,pwd)" "VALUES (:name,:pwd)");
    query.bindValue(":name", logonUsrName);
    query.bindValue(":pwd", logonPwd);
    query.exec();
}

bool Widget::checkUsr()
{
    QSqlQuery query;
    query.exec("select * from usr ORDER BY id ASC");
    while(query.next())
    {
        if (logonUsrName==query.value(1).toString())
            return 0;

        /*?
        if (db.driver()->hasFeature(QSqlDriver::QuerySize))
        {
           numRows = query.size();
        }
        else
        {
           // this can be very slow
           query.last();
           numRows = query.at() + 1;
        }
        */

    }
    return 1;
}
/*
void Widget::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
      {
          case QAbstractSocket::RemoteHostClosedError : break;
          default : qDebug() << logonConnection->errorString();
      }
}
*/
/*
void handOfflineFile()
{
}
*/
