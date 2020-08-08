#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT
signals:
    //关闭窗口发送关闭信息
    void closeLogin();
public:
    //关闭的事件
    void closeEvent(QCloseEvent* );

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;
    //QString name;
    //QString image;
};

#endif // LOGIN_H
