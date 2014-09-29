#include "tcpreceive.h"
#include "ui_tcpreceive.h"
#include <QFileDialog>
#include <QMessageBox>

TcpReceive::TcpReceive(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpReceive)
{
    ui->setupUi(this);
    this->setFixedSize(350,180);

        TotalBytes = 0;
        bytesReceived = 0;
        fileNameSize = 0;

        tcpClient = new QTcpSocket(this);
        tcpPort = 7317;
        connect( tcpClient,SIGNAL(readyRead()),this,SLOT(readMessage()));
        connect( tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
                SLOT(displayError(QAbstractSocket::SocketError)));

}

TcpReceive::~TcpReceive()
{
    delete ui;
}

void TcpReceive::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TcpReceive::setHostAddress(QHostAddress address)
{
    hostAddress = address;
    newConnect();
}
void TcpReceive::newConnect()
{
    blockSize = 0;
    tcpClient->abort();
    tcpClient->connectToHost(hostAddress,tcpPort);
    time.start();
}

void TcpReceive::readMessage()
{
    QDataStream in(tcpClient);
    in.setVersion(QDataStream::Qt_4_6);

    float useTime = time.elapsed();
    if(bytesReceived <= sizeof(qint64)*2){
        if((tcpClient->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0)){
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tcpClient->bytesAvailable() >= fileNameSize) && (fileNameSize != 0)){
            in>>fileName;
            bytesReceived +=fileNameSize;

            if(!localFile->open(QFile::WriteOnly)){
                QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件 %1:\n%2.").arg(fileName).arg(localFile->errorString()));
                return;
            }
        }else{
            return;
        }
    }
    if(bytesReceived < TotalBytes){
        bytesReceived += tcpClient->bytesAvailable();
        inBlock = tcpClient->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesReceived);
    qDebug()<<bytesReceived<<"received"<<TotalBytes;

    double speed = bytesReceived / useTime;
    ui->tcpClientStatusLabel->setText(tr("已接收 %1MB (%2MB/s) \n共%3MB 已用时:%4秒\n估计剩余时间：%5秒")
                                   .arg(bytesReceived / (1024*1024))
                                   .arg(speed*1000/(1024*1024),0,'f',2)
                                   .arg(TotalBytes / (1024 * 1024))
                                   .arg(useTime/1000,0,'f',0)
                                   .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));

    if(bytesReceived == TotalBytes)
    {
        tcpClient->close();
        ui->tcpClientStatusLabel->setText(tr("接收文件 %1 完毕").arg(fileName));
    localFile->close();
    }
}


void TcpReceive::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError : break;
        default : qDebug() << tcpClient->errorString();
    }
}
void TcpReceive::on_tcpClientCloseBtn_clicked()
{
    tcpClient->abort();
    this->close();
}
void TcpReceive::closeEvent(QCloseEvent *)
{
    on_tcpClientCloseBtn_clicked();
}

void TcpReceive::on_tcpClientCancelBtn_clicked()
{
    tcpClient->abort();
}

