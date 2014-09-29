#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include <QtNetwork>
#include <QtGui>
#include "tcpreceive.h"
#include "tcpsend.h"

namespace Ui {
    class chat;
}
enum MessageType
{
    Message,
    NewParticipant,
    ParticipantLeft,
    FileName,
    Refuse,
    Xchat
};
class chat : public QDialog
{
    Q_OBJECT

public:
    ~chat();
    //chat();
    chat(QString pasvusername, QString pasvuserip);
    QString xpasvuserip;
    QString xpasvusername;
    QUdpSocket *xchat;
      qint32 xport;
      void sendMessage(MessageType type,QString serverAddress="");
      quint16 a;
  //    static  qint32 is_opened = 0;
      bool is_opened;
protected:
    void hasPendingFile(QString userName,QString serverAddress,  //接收文件
                                QString clientAddress,QString fileName);
    void participantLeft(QString userName,QString time);
    //bool eventFilter(QObject *target, QEvent *event); //事件过滤器
private:
    Ui::chat *ui;
    TcpSend *server;
    QColor color;//颜色
    bool saveFile(const QString& fileName);//保存聊天记录
    QString getMessage();
    QString getIP();
    QString getUserName();
    QString message;
    QString fileName;
private slots:
    void sentFileName(QString);
    void on_sendfile_clicked();
    void processPendingDatagrams();
    void on_send_clicked();
    void on_close_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void on_textcolor_clicked();
    void on_textUnderline_clicked(bool checked);
    void on_textitalic_clicked(bool checked);
    void on_textbold_clicked(bool checked);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_fontsizecomboBox_currentIndexChanged(QString );
    void currentFormatChanged(const QTextCharFormat &format);
};

#endif // CHAT_H
