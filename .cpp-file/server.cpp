#include "server.h"
#include "ui_server.h"
#include<QMessageBox>
#include<QDateTime>

server::server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::server)
{
    ui->setupUi(this);

    udpsocket=new QUdpSocket(this);
    //设置端口号
    sport=40000;
    //开启服务器
    connect(ui->startbtn,&QPushButton::clicked,[=](){
        udpsocket->bind(this->sport,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    });

    //监听别人发送的数据
    connect(udpsocket,&QUdpSocket::readyRead,this,&server::ReceiveMessage);

    //点击退出按钮 实现退出服务器功能
    connect(ui->closebtn,&QPushButton::clicked,[=](){
        this->close();
    });
}

//接收客户端消息
void server::ReceiveMessage()
{
    //拿到数据报文
    qint64 size=udpsocket->pendingDatagramSize();   //获取长度
    QByteArray array=QByteArray(size,0);
    udpsocket->readDatagram(array.data(),size);   //获取数据

    //解析数据
    //第一段  类型   第二段 用户名   第三段 具体内容
    QDataStream stream(&array,QIODevice::ReadOnly);
    int msgType;   //读取到类型
    QString usrName;
    QString msg;

    //获取当前时间
    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    stream>>msgType;

    switch(msgType){
    case Msg:       //普通聊天  直接将内容传给所有客户端
        udpsocket->writeDatagram(array,QHostAddress::Broadcast,9999);
        break;
    case UsrEnter:     //新用户进入
        {
            stream>>usrName;
            bool isempty=ui->xingmin->findItems(usrName,Qt::MatchExactly).isEmpty();  //看他是否在线
            if(isempty)
            {
                QTableWidgetItem* usr=new QTableWidgetItem(usrName);
                //插入行
                ui->xingmin->insertRow(0);
                ui->xingmin->setItem(0,0,usr);
            }
            else
            {
                return;
            }

            //让后来登陆的用户界面也能显示所有在线的用户
            //这里是根据服务端的用户名列表  来向各个客户端发送用户在线信息，该方法应该比在客户端发送要更好
            //因为服务端是1对多发送，而客户端中每个用户都是一个主体，依次向其他客户端发
            //在服务端中，不管count是从第0行开始还是列表最后一行开始，这两行都必须放在循环内部，否则达不到效果
            qint32 count=(ui->xingmin->rowCount())-1;
            while(count>=0)
            {
                QByteArray str;
                QDataStream stream(&str,QIODevice::WriteOnly);           //添加内容到str中
                QString content=ui->xingmin->item(count,0)->text();      //获得用户列表中每行的值
                stream<<UsrEnter<<content;                            //将类型和用户名内容添加到流中
                udpsocket->writeDatagram(str,QHostAddress::Broadcast,9999);     //改成给客户端发
                --count;
            }

            break;
        }
    case UsrLeft:    //用户退出
        {
            stream>>usrName;
            bool isempty=ui->xingmin->findItems(usrName,Qt::MatchExactly).isEmpty();
            if(!isempty)
            {
                int row=ui->xingmin->findItems(usrName,Qt::MatchExactly).first()->row();
                ui->xingmin->removeRow(row);
            }
            udpsocket->writeDatagram(array,QHostAddress::Broadcast,9999);
            break;
        }
    default:
        break;
    }
}


//关闭服务器
void server::closeEvent(QCloseEvent* )
{
    emit this->closeserver();
    //断开套接字
    udpsocket->close();
    udpsocket->destroyed();
}

server::~server()
{
    delete ui;
}
