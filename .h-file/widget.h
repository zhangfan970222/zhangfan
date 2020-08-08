#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QUdpSocket>
#include<QtSql>
#include<QSqlDatabase>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    enum MsgType{Msg,UsrEnter,UsrLeft};
public:
    explicit Widget(QWidget *parent = nullptr,QString name="");
    ~Widget();

private:
    Ui::Widget *ui;
signals:
    //关闭窗口发送关闭信息
    void closeWidget();
public:
    //连接数据库函数
    QSqlQuery connect_mysql();

    //关闭的事件
    void closeEvent(QCloseEvent* );

    void sndMsg(MsgType type);    //广播UDP消息
    void ReceiveMessage();    //接收udp信息
    void usrEnter(QString username);   //处理新用户加入
    void usrLeft(QString usrname,QString time);   //处理用户离开
    QString getUsr();   //获取用户名
    QString getMsg();   //获取聊天记录
private:
    QUdpSocket* udpsocket;   //udp套接字
    qint16 port;  //端口号
    QString uName;   //用户名
    QByteArray array;  //存放发送和接收的数据
    QSqlQuery query;   //数据库对象

};

#endif // WIDGET_H
