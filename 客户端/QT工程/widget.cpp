#include "widget.h"
#include "ui_widget.h"
#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include <QTableView>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QPalette>
#include "loginName.h"
QString loginName;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    initChatRoom(this); //初始化聊天室界面


    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/imgs/imgs/chatRoomBk.png")));
    this->setPalette(palette);

    udpSocket = new QUdpSocket(this);

    receiver=new QUdpSocket(this);
    port1 = 6919;
    port2= 8919;
    //xport =7777;
    bb=0;
    privatechat = NULL;
    privatechat1 = NULL;
    //改成固定服务端地址
    qDebug() << "client UDP";
    receiver->bind(port2,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint); //用户端绑定到服务端，ShareAddress允许多用户绑定到相同的服务端地址和端口，ReuseAddressHint为重新连接服务器
    connect(receiver, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));//服务端一传来消息即调用processPendingDatagrams()进行处理
    sendMessage(NewParticipant); //进入聊天室即发送新用户消息

    server = new TcpSend(this); //发送文件的服务端类
    connect(server, SIGNAL(sendFileName(QString)), this, SLOT(getFileName(QString))); //一收到传来文件的消息即进行接受处理
    connect(ui->messageTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this,
                SLOT(currentFormatChanged(QTextCharFormat)));//改变文字样式
}

Widget::~Widget()
{
    delete ui;
}
/*
void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    QPalette pal(palette());

// 实现大小拉伸
    pal.setBrush(QPalette::Window,
                QBrush(bkImg.scaled(event->size(), Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation)));
    setPalette(pal);
}
*/


void Widget::initChatRoom(Widget* ts)
{
/*
    /new new
       bkImg.load("imgs/chatRoomImg.ipg");
        ts->setAutoFillBackground(true);

       ts->setStyleSheet("#QWidget{background-image: url(:/imgs/imgs/chatRoomImg.jpg);}");

     //new
        ui->messageTextEdit->setFocusPolicy(Qt::StrongFocus);
        ui->messageBrowser->setFocusPolicy(Qt::NoFocus);

        ui->messageTextEdit->setFocus();
        ui->messageTextEdit->installEventFilter(ts);//设置完后自动调用其eventFilter函数

    //改变Widget窗口的背景色和文字颜色

    QPalette pal = ts->palette();
    //QString filename = QDir::currentPath()+ "imgs/roundrect.jpg";
    //QPixmap img(filename);
    //pal.setBrush(QPalette::Window, QBrush(img));
    //pal.setColor(QPalette::Background, QColor(211,211,211));
    pal.setColor(QPalette::Text, Qt::white);
    ts->setPalette(pal);

    //改变聊天消息浏览框
    ui->messageBrowser->setStyleSheet("QTextBrowser{border:0px groove gray;border-radius:5px;color:white}");
    //改变用户列表框
    ui->userTableWidget->setStyleSheet("QTableWidget{border:0px groove gray;border-radius:5px;color:white}");
    //改变输入框
    ui->messageTextEdit->setStyleSheet("QTextEdit{border:2px groove gray;border-radius:5px;} ");

    //改变右侧用户列表样式
    QPalette pal4 =  ui->userTableWidget->palette();
   // pal4.setColor(QPalette::Base, Qt::black);
   // pal4.setBrush(ts->backgroundRole(),QBrush(QColor(255,255,255)) );
    ui->userTableWidget->setPalette( pal4 );
    ui->userTableWidget->verticalHeader()->setVisible(0); //隐藏每一行的序号
    ui->userTableWidget->horizontalHeader()->setStretchLastSection(true); //表头最后一项自动填充
    ui->userTableWidget->horizontalHeader()->setStyleSheet(tr("QHeaderView::section {background-color:#e0e0e0;color:rgb(200,111,30)}"));//改变表头文字颜色

    //改变输入框上方的选项框样式
    ui->groupBox->setStyleSheet( "border:0px groove gray;border-radius:1px;");  //圆角边框
*/    //发送按钮

    ui->sendButton->setStyleSheet(tr("QPushButton{ border:0px;border-radius:5px;color:white;background-color:rgb(172, 213, 191)}QPushButton:hover{background-color:rgb(153, 186, 255);}QPushButton:pressed{background-color:rgb(153, 186, 255);}"));
    //退出按钮
    ui->exitButton->setStyleSheet(
    tr("QPushButton{border:0px;border-radius:5px;color:white;background-color:rgb(195, 195, 195);}QPushButton:hover{background-color:#505050;}QPushButton:pressed{background-color:#505050;}"));


    //私聊按钮
   // ui->privatChat->setStyleSheet(
    //tr("QPushButton{color:#e0e0e0;border:2px groove #505050;border-radius:4px;background-color:gray;}QPushButton:hover{background-color:#f0f0f0;color:gray}QPushButton:pressed{background-color:#f0f0f0;color:gray;}"));

}

void Widget::sendMessage(MessageType type, QString serverAddress)
{
    
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();//本机用户名
    QString address = getIP();//ip地址


    out << type << getUserName() << localHostName;//首先输出消息类型
    switch(type)
    {
    case Message: //聊天信息
        qDebug()<<"sendMessage message";
        if(ui->messageTextEdit->toPlainText() == "")
        {
            QMessageBox::warning(0, tr("Warning"), tr("发送内容不能为空"), QMessageBox::Ok);
            return;
        }
        out  << address << getMessage(); //获取输入内容并发送
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageBrowser->verticalScrollBar()->maximum());
        break;

    case NewParticipant://新加入用户
        qDebug()<<"sendMessage new";
        out << address;
        break;

    case ParticipantLeft://离开
        qDebug()<<"sendMessage left";
        break;

    case FileName://发送文件命令
    {
        qDebug()<<"sendMessage FileName";
        int row = ui->userTableWidget->currentRow(); //被发送对象所在的用户列表行
        QString clientAddress = ui->userTableWidget->item(row, 2)->text(); //获取被发送对象地址
        out << address << clientAddress << fileName;
        break;
    }

    case Refuse://拒绝接受文件
    {
        qDebug()<<"sendMessage Refuse";
        out << serverAddress;
        break;

    }


    }

    udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port1);//写入客户端

}

void Widget::processPendingDatagrams()
{
    qDebug() << "client process";
    while(receiver->hasPendingDatagrams())  //至少有一个数据包正被读取
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());//读取消息

        QDataStream in(&datagram, QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        qDebug() << messageType;
        QString userName, localHostName, ipAddress, message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");//读取时间

        switch(messageType)
        {
        case Message://更新聊天消息列表
            in >> userName >> localHostName >> ipAddress >> message;
            ui->messageBrowser->setTextColor(QColor(7,151,237));
            ui->messageBrowser->setCurrentFont(QFont("宋体", 12));
            ui->messageBrowser->append("[" + userName + "]" + time);
            ui->messageBrowser->append(message);
            break;

        case NewParticipant://更新用户列表
            qDebug() << "client process new";
            in >> userName >> localHostName >> ipAddress;
            qDebug() << userName << localHostName << ipAddress;
            newParticipant(userName, localHostName, ipAddress);
            break;

        case ParticipantLeft://处理用户离开
             qDebug() << "client process left";
            in >> userName >> localHostName;
            participantLeft(userName, localHostName, time);
            break;

        case FileName://发送文件窗口显示
        {
            in >> userName >> localHostName >> ipAddress;
            QString clientAddress, fileName;
            in >> clientAddress >> fileName;
            hasPendingFile(userName, ipAddress, clientAddress, fileName);
            break;
        }

        case Refuse://对发拒收文件
         {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();
            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
case Xchat:
            {
                in >>userName >>localHostName >>ipAddress;
                showxchat(userName,ipAddress);
                break;
            }
        }//switch
    }//while
}

void Widget::newParticipant(QString userName, QString localHostName, QString ipAddress)
{
    bool isEmpty = ui->userTableWidget->findItems(userName, Qt::MatchExactly).isEmpty();//用户列表是否没有当前用户

    if(isEmpty)//加入用户
    {
        int myRow=ui->userTableWidget->rowCount();
        QTableWidgetItem* user = new QTableWidgetItem(userName);
        QTableWidgetItem* host = new QTableWidgetItem(localHostName);
        QTableWidgetItem* ip = new QTableWidgetItem(ipAddress);

        ui->userTableWidget->insertRow(myRow);
        ui->userTableWidget->setItem(myRow, 0, user);
        ui->userTableWidget->setItem(myRow, 1, host);
        ui->userTableWidget->setItem(myRow, 2, ip);

        ui->messageBrowser->setTextColor(Qt::gray);
        ui->messageBrowser->setCurrentFont(QFont("宋体", 10));
        ui->messageBrowser->append(tr(" %1 在线!").arg(userName));
        ui->userNumLabel->setText(tr("在线人数 : %1 ").arg(ui->userTableWidget->rowCount()));

        sendMessage(NewParticipant);
    }
}

void Widget::participantLeft(QString userName, QString localHostName, QString time)
{
    int rowNum = ui->userTableWidget->findItems(userName, Qt::MatchExactly).first()->row();//找到离开用户所在行，删除

    ui->userTableWidget->removeRow(rowNum);
    ui->messageBrowser->setTextColor(Qt::gray);
    ui->messageBrowser->setCurrentFont(QFont("宋体", 10));

    ui->messageBrowser->append(tr(" %1 于 %2 离开!").arg(userName).arg(time));//显示离开消息
    ui->userNumLabel->setText(tr("在线人数 : %1 ").arg(ui->userTableWidget->rowCount()));
}


QString Widget::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
      foreach (QHostAddress address, list)
      {
          //使用IPv4地址
          if(address.protocol() == QAbstractSocket::IPv4Protocol)
          {
              if (address.toString().contains("127.0.0.1")||address.toString().contains("169.")) //避免显示127.0.0.1
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

QString Widget::getUserName() //获得用户名
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*" << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();

    foreach(QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if(index != -1)
        {
            QStringList stringList = environment.at(index).split("=");
            if(stringList.size() == 2)
            {
                return loginName;
                break;
            }
        }
    }

    return "unknown";
}

QString Widget::getMessage()
{
    QString msg = ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();
    return msg;
}



void Widget::getFileName(QString name)//获得文件名
{
    fileName = name;
    sendMessage(FileName);
}


void Widget::hasPendingFile(QString userName, QString serverAddress, QString clientAddress, QString fileName)//接受文件处理
{
    QString ipAddress = getIP();
    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this, tr("接收文件"), tr("来自 %1 (%2)的文件: %3, 是否接收?")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes, QMessageBox::No);
        if(btn == QMessageBox::Yes)
        {
            QString name = QFileDialog::getSaveFileName(0, tr("保存文件"), fileName);
            if(!name.isEmpty())
            {
                TcpReceive *client = new TcpReceive(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        }
        else
        {
            sendMessage(Refuse, serverAddress);
        }
    }
}
//new

void Widget::on_sendButton_clicked()//发送消息
{
    sendMessage(Message);
}

void Widget::on_sendToolBtn_clicked()//发送文件按钮
{
    if(ui->userTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, tr("选择用户"), tr("请先从用户列表中选择要传送的用户!!!"), QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();
}

void Widget::on_save_clicked()//保存聊天记录
{
    if(ui->messageBrowser->document()->isEmpty())
        QMessageBox::warning(0,tr("warning"),tr("聊天记录为空，无法保存！"),QMessageBox::Ok);
    else
    {
       //获得文件名,注意getSaveFileName函数的格式即可
       QString fileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录"),tr("文本(*.txt);;All File(*.*)"));
       if(!fileName.isEmpty())
           saveFile(fileName);
    }
}

bool Widget::saveFile(const QString &fileName)//保存文件
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))

    {
        QMessageBox::warning(this,tr("保存文件"),
        tr("无法保存文件 %1:\n %2").arg(fileName)
        .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->messageBrowser->toPlainText();

    return true;
}

void Widget::on_fontComboBox_currentFontChanged(const QFont &f)//改变字体
{
    ui->messageTextEdit->setCurrentFont(f);
    ui->messageTextEdit->setFocus();
}

void Widget::on_sizeComboBox_currentIndexChanged(const QString &arg1)//改变字体大小
{
    ui->messageTextEdit->setFontPointSize(arg1.toDouble());
    ui->messageTextEdit->setFocus();
}

void Widget::on_boldToolBtn_clicked(bool checked)//粗体
{
    if(checked)
    {
        ui->messageTextEdit->setFontWeight(QFont::Bold);
    }
    else
    {
        ui->messageTextEdit->setFontWeight(QFont::Normal);
    }
    ui->messageTextEdit->setFocus();
}


void Widget::on_italicToolBtn_clicked(bool checked)//斜体
{
    ui->messageTextEdit->setFontItalic(checked);
    ui->messageTextEdit->setFocus();
}


void Widget::on_underlineToolBtn_clicked(bool checked)//下划线
{
    ui->messageTextEdit->setFontUnderline(checked);
    ui->messageTextEdit->setFocus();
}

void Widget::on_colorToolBtn_clicked()//字体颜色
{
    color = QColorDialog::getColor(color, this);
    if(color.isValid())
    {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}

void Widget::currentFormatChanged(const QTextCharFormat &format)//改变选项栏各控件状态
{
    ui->fontComboBox->setCurrentFont(format.font());
    if(format.fontPointSize() < 9)
    {
        ui->sizeComboBox->setCurrentIndex(3); //默认为12
    }
    else
    {
        ui->sizeComboBox->setCurrentIndex(ui->sizeComboBox->findText(QString::number(format.fontPointSize())));
    }
    ui->boldToolBtn->setChecked(format.font().bold());
    ui->italicToolBtn->setChecked(format.font().italic());
    ui->underlineToolBtn->setChecked(format.font().underline());
    color = format.foreground().color();
}

void Widget::on_clearToolBtn_clicked()//清除聊天消息
{
    ui->messageBrowser->clear();
}

//new
void Widget::on_userTableWidget_doubleClicked(QModelIndex index)
{
    if(ui->userTableWidget->item(index.row(),0)->text() == getUserName() &&
        ui->userTableWidget->item(index.row(),2)->text() == getIP())
    {
        QMessageBox::warning(0,tr("精分预警"),tr("跟自己聊天？"),QMessageBox::Ok);
    }
    else
    {
    //    else
        if(!privatechat){
      //  chat *privatechatTemp;
        privatechat = new chat(ui->userTableWidget->item(index.row(),1)->text(), //接收主机名
                               ui->userTableWidget->item(index.row(),2)->text()) ;//接收用户IP
        }
        qDebug()<< (ui->userTableWidget->item(index.row(),1)->text()) << (ui->userTableWidget->item(index.row(),2)->text());
//        if( privatechat->is_opened )delete privatechat;//如果其曾经显示过则删除掉
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        QString localHostName = QHostInfo::localHostName();
        QString address = getIP();
        out << Xchat << getUserName() << localHostName << address;
        QHostAddress chatAddr((ui->userTableWidget->item(index.row(),2)->text()));

        udpSocket->writeDatagram(data,data.length(),chatAddr, port1);

//        privatechat->xchat->writeDatagram(data,data.length(),QHostAddress::QHostAddress(ui->tableWidget->item(index.row(),2)->text()), 45456);
      //  if(!privatechat->is_opened)
            privatechat->show();
        privatechat->is_opened = true;
    //    (privatechat->a) = 0;
    }

}
//new
void Widget::showxchat(QString name, QString ip)
{
    qDebug() << "show xchat";
//    if(!privatechat){
 // chat *privatechatTemp;
    if(!privatechat1)
    privatechat1 = new chat(name,ip);
//    privatechat = privatechatTemp;}
//    chat privatechat(name,ip);//如果不用new函数，则程序运行时只是闪烁显示一下就没了，因为类的生命周期结束了
//    privatechat->is_opened = false;
 // privatechat->show();
  //privatechat.textBrowser.show();
  //privatechat->is_opened = true;
    bb++;
    //delete privatechat;

}



void Widget::on_exitButton_clicked()//退出
{
    close();
}
//new
void Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void Widget::closeEvent(QCloseEvent *e)
{
    sendMessage(ParticipantLeft);
    QWidget::closeEvent(e);
}
/*
void Widget::on_logoutButton_clicked()
{
    sendMessage(ParticipantLeft);
    newLogin=new login(this);
    newLogin->show();
    this->close();
}
*/
