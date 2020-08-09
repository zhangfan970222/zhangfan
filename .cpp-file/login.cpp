#include "login.h"
#include "ui_login.h"
#include"widget.h"
#include<QMessageBox>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    //设置标题
    setWindowTitle("WeChat");
    //设置图标
    setWindowIcon(QIcon(":/images/qq.png"));

/*************
并没有用
    //用户名字
    name=ui->nametext->currentText();
    //头像
    image=ui->imagetext->currentText();
*************/

    //点击登陆
    connect(ui->pushbtn,&QPushButton::clicked,[=]() {
/*********************
        if(name.isEmpty())
        {
            QMessageBox::warning(this,"警告:","用户名不能为空");
            return;
        }
        if(name.length()>10)
        {
            QMessageBox::warning(this,"警告:","用户名过长，最大长度为10");
            return;
        }
***********************/

        
        //弹出对话框
        Widget* widget=new Widget(nullptr,ui->nametext->currentText());  //传递给widget的名字,显示上线下线用
        //设置窗口标题，头像
        widget->setWindowTitle(ui->nametext->currentText());                    //传递给widget窗口的名字
        widget->setWindowIcon(QPixmap(ui->imagetext->currentText()));               //传递给widget窗口的头像
        widget->show();
        this->close();
    });

    //点击取消
    connect(ui->cancelbtn,&QPushButton::clicked,[=]() {
         this->close();
    });
}

void Login::closeEvent(QCloseEvent* )
{
    emit this->closeLogin();
}


Login::~Login()
{
    delete ui;
}
