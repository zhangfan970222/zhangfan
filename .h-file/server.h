#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include<QUdpSocket>

namespace Ui {
class server;
}

class server : public QWidget
{
    Q_OBJECT
    enum MsgType{Msg,UsrEnter,UsrLeft};
public:
    explicit server(QWidget *parent = nullptr);
    ~server();

signals:
    //关闭窗口发送关闭信息
    void closeserver();
public:
    //关闭的事件
    void closeEvent(QCloseEvent* );

    void ReceiveMessage();

private:
    Ui::server *ui;
    QUdpSocket* udpsocket;   //udp套接字
    qint16 sport;  //端口号
};

#endif // SERVER_H
