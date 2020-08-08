#include "widget.h"
#include "ui_widget.h"
#include<QMessageBox>
#include<QDateTime>
#include<QColorDialog>
#include<QFileDialog>
#include<QFile>
#include<QTextStream>


Widget::Widget(QWidget *parent,QString name) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //初始化数据库
    query=connect_mysql();

    //初始化操作
    udpsocket=new QUdpSocket(this);
    //用户名获取
    uName=name;
    //端口号
    this->port=9999;

    //绑定端口号      绑定模式，共享地址，断线重连
    udpsocket->bind(this->port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    //发现新用户进入
    sndMsg(UsrEnter);


    //点击发送按钮发送消息
    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        sndMsg(Msg);
    });

    //监听别人发送的数据
    connect(udpsocket,&QUdpSocket::readyRead,this,&Widget::ReceiveMessage);

    //点击退出按钮 实现退出功能
    connect(ui->exitBtn,&QPushButton::clicked,[=](){
        this->close();
    });


    //辅助功能
    //字体
    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->msgTxtEdit->setCurrentFont(font);
        ui->msgTxtEdit->setFocus();//让光标回到输入框内,可以继续输入
    });
    //字号
    void(QComboBox::*cbxsingal)(const QString&text)=&QComboBox::currentIndexChanged;  //该函数有重载，先得到该函数重载类型const QString&类型的函数指针。
    connect(ui->sizeCbx,cbxsingal,[=](const QString&text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgTxtEdit->setFocus();
    });
    //加粗
    connect(ui->boldTBtn,&QToolButton::clicked,[=](bool isCheck){
        if(isCheck)
            ui->msgTxtEdit->setFontWeight(QFont::Bold);
        else {
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
        }
    });
    //倾斜
    connect(ui->italicTBtn,&QToolButton::clicked,[=](bool check){
        ui->msgTxtEdit->setFontItalic(check);
    });
    //下划线
    connect(ui->underlineTBtn,&QToolButton::clicked,[=](bool check){
        ui->msgTxtEdit->setFontUnderline(check);
    });
    //字体颜色
    connect(ui->colorTBtn,&QToolButton::clicked,[=](){
        QColor color=QColorDialog::getColor(Qt::red);
        ui->msgTxtEdit->setTextColor(color);
    });
    //清空聊天记录
    connect(ui->clearTbtn,&QToolButton::clicked,[=](){
        ui->msgBrowser->clear();
    });

    //保存聊天记录
    connect(ui->saveTBtn,&QToolButton::clicked,[=](){
        if(ui->msgBrowser->document()->isEmpty())
        {
            QMessageBox::warning(this,"警告：","内容不能为空");
            return;
        }
        else
        {
            QString path=QFileDialog::getSaveFileName(this,"保存记录","聊天记录","(*.txt)");
            if(path.isEmpty())
            {
                QMessageBox::warning(this,"警告：","保存失败");
                return;
            }
            QFile file(path);
            //打开模式加换行操作
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream stream(&file);
            stream<<ui->msgTxtEdit->toPlainText();
            file.close();
        }
    });
}

//连接数据库函数
QSqlQuery Widget::connect_mysql()
{
    qDebug() << QSqlDatabase::drivers();

    //addDatabase()的第一个参数
    //指定了Qt必须使用哪一个数据库驱动程序来访问这个数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    db.setPort(3306);                 //连接数据库端口号，与设置一致
    db.setDatabaseName("wechat");      //连接数据库名，与设置一致
    db.setUserName("root");          //数据库用户名，与设置一致
    db.setPassword("zf701220@");    //数据库密码，与设置一致
    db.open();

    if(!db.open())
    {
      qDebug()<<"不能连接"<<"connect to mysql error"<<db.lastError().text();
    }
    else
    {
     qDebug()<<"连接成功"<<"connect to mysql OK";
    }
    QSqlQuery query(db);
    /*
    query.exec("select * from record");
    while(query.next())
   {
     qDebug()<<query.value("name").toString();
     qDebug()<<query.value("time").toInt();
     qDebug()<<query.value("valu").toString();
   }
   */
    return query;
}



void Widget::ReceiveMessage()
{
    //拿到数据报文
    qint64 size=udpsocket->pendingDatagramSize();   //获取长度
    array=QByteArray(size,0);
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
    case Msg:       //普通聊天
    {
        stream>>usrName>>msg;
        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue);
        ui->msgBrowser->append("[" + usrName+"]" + time);
        ui->msgBrowser->append(msg);

        //插入变量数据到数据库，方法一
        QString str=QObject::tr("insert into record(`name`,`time`,valu) values('%1','%2','%3')").arg(usrName,time,msg);
        query.exec(str);
        //方法二：
        //query.prepare("insert into record(`name`,`time`,valu) values(:n1,:t1,:v1)");
        //query.bindValue(":n1",usrName);
        //query.bindValue(":t1",time);
        //query.bindValue(":v1",msg);
        //query.exec();

        //检测是否插入成功
        query.exec("select * from record");
        while(query.next())
       {
         qDebug()<<query.value("name").toString();
         qDebug()<<query.value("time").toInt();
         qDebug()<<query.value("valu").toString();
       }

        break;
    }
    case UsrEnter:
        stream>>usrName;
        usrEnter(usrName);

        break;
    case UsrLeft:
        stream>>usrName;
        usrLeft(usrName,time);
        break;
    default:
        break;
    }
}

//广播udp消息
void Widget::sndMsg(MsgType type)
{
    //发送的消息分为3种类型
    //发送的数据做分段处理，第一段类型，第二段用户名   第三段具体内容
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);     //添加内容到array中

    stream<<type<<getUsr();    //第一段和第二段内容添加到流中

    switch(type){
    case Msg:         //普通消息发送
        {
            QString str=ui->msgTxtEdit->toPlainText();
            qint32 len=ui->msgTxtEdit->toPlainText().length();

            //如果用户没有输入内容，则不发送任何信息
            if(str=="")
            {
                QMessageBox::warning(this,"警告:","发送内容不能为空");
                ui->msgTxtEdit->clear();
                ui->msgTxtEdit->setFocus(); //让光标回到输入框内,可以继续输入
                return;
            }

            //第一个字符不能是回车
            if(str[0]=="\n")
            {
                QMessageBox::warning(this,"警告:","第一行不能为空");
                //清空输入框
                ui->msgTxtEdit->clear();
                ui->msgTxtEdit->setFocus(); //让光标回到输入框内,可以继续输入
                return;
            }
            int count=0;
            for(int j=len-1;j>=0;--j)
            {
                if(str[j]=="\n")
                {
                    ++count;
                }
                else
                {
                    break;
                }
            }
            if(count>0)
            {
                QMessageBox::warning(this,"警告:","最后一行不能为空");
                //让光标回到输入框内,可以继续输入
                ui->msgTxtEdit->setFocus();
                return;
            }

            //不能全是空格
            int i=0;
            for( ;i<len;++i)
            {
                if(str[i]==" ")
                {
                    ;
                }
                else
                {
                   break;
                }
            }
            if(i==len)
            {
                QMessageBox::warning(this,"警告:","发送内容不能为空");
                //清空输入框
                ui->msgTxtEdit->clear();
                ui->msgTxtEdit->setFocus(); //让光标回到输入框内,可以继续输入
                return;
            }

            //最大长度20
            if(len>20)
            {
                QMessageBox::warning(this,"警告:","发送内容过长，最大长度20");
                //让光标回到输入框内,可以继续输入
                ui->msgTxtEdit->setFocus();
                return;
            }

            //第三段具体内容
            stream<<getMsg();
            break;
        }
    case UsrEnter:        //新用户进入消息

        break;
    case UsrLeft:            //用户离开消息

        break;
    default:
        break;
    }

    //书写报文(发送)，是广播
    udpsocket->writeDatagram(array,QHostAddress::Broadcast,40000);
}

//获取聊天信息
QString Widget::getMsg()
{
    QString str=ui->msgTxtEdit->toHtml();
    //清空输入框
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus(); //让光标回到输入框内,可以继续输入
    return str;
}

//获取用户名
QString Widget::getUsr()
{
    return this->uName;
}

//处理新用户加入
void Widget::usrEnter(QString username)
{
    //更新右侧的tablewidget
    bool isempty=ui->usrTblWidget->findItems(username,Qt::MatchExactly).isEmpty();  //看他是否在线
    if(isempty)
    {
        QTableWidgetItem* usr=new QTableWidgetItem(username);
        //插入行
        ui->usrTblWidget->insertRow(0);
        ui->usrTblWidget->setItem(0,0,usr);

        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QString("%1 上线了!").arg(username));

        //在线人数更新
        ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTblWidget->rowCount()));
        //以上都是在别的用户界面显示，自己界面未显示

/*******
        //让后来登陆的用户界面也能显示所有在线的用户

        //添加内容到str中，这两行代码放在while里面或者在外面都能实现,因为对于后加入的用户来说：在他前面加入的用户
        他的用户列表中未显示，但是我们的count从该用户列表的最后一行开始，也就是str中的内容是之前登陆的用户的例表中
        的最后一个人，经过每次循环，str内容永远都是 (UsrEnter + 最后一行用户名)   +  (UsrEnter + 倒数第二行用户名)....
        其中有效的还是   UsrEnter + 最后一行用户名,因为每次有新用户加入时，老用户列表都会将新用户名放在 0 行的位置
        //但是如果count从0开始的话for(int i=0;i<count;++i)，这两行必须放在循环内部，否则无效。


        qint32 count=ui->usrTblWidget->rowCount();
        for(int i=0;i<count;++i)
        {
            QByteArray str;
            QDataStream stream(&str,QIODevice::WriteOnly);        //  count从0开始，必须放在循环内部
            QString content=ui->usrTblWidget->item(i,0)->text();  //获得用户列表中每行的值
            stream<<UsrEnter<<content;    //将类型和用户名内容添加到流中
            udpsocket->writeDatagram(str,QHostAddress::Broadcast,9999); //给该网段的所有客户端发
        }
*******/

    }
}

//处理用户离开
void Widget::usrLeft(QString usrname,QString time)
{
    //更新右侧的tableWidget
    bool isempty=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(!isempty)
    {
        int row=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).first()->row();
        ui->usrTblWidget->removeRow(row);

        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QString("%1 在 %2 离开了!").arg(usrname).arg(time));

        //在线人数更新
        ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTblWidget->rowCount()));
    }
}

void Widget::closeEvent(QCloseEvent* )
{
    emit this->closeWidget();
    sndMsg(UsrLeft);
    //断开套接字
    udpsocket->close();
    udpsocket->destroyed();
}


Widget::~Widget()
{
    delete ui;
}
