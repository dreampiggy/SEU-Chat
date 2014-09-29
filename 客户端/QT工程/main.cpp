#include <QtGui/QApplication>
#include "widget.h"
#include "login.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    QApplication a(argc, argv);
    login privateLogin;
    privateLogin.show();
    if(privateLogin.exec()==QDialog::Accepted)
    {
        Widget chatRoom;
        chatRoom.show();
        return a.exec();
    }
    else
        return 0;

    /*Widget w;
    w.show();
    return a.exec();
    */
}
