/*
 !to do!
 0.（用户状态！）
 1.将用户显示为登录名
 2.私聊,发送文件以登录名为判别条件
 3.添加好友列表，加入friends数据库
 4.好友间才可以私聊
 5.私聊自动回复
 6.截图工具
 7.管理员登陆
 8.离线消息
*/

#include <QtGui/QApplication>
#include "widget.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
