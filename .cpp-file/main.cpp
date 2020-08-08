#include "widget.h"
#include"login.h"
#include"server.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.show();

     Login list;
     list.show();

     server ser;
     ser.show();


    return a.exec();
}
