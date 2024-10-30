#include "server.h"
#include "operatedb.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDb::getInstance().connect();
    Server w;
//    w.show();
    return a.exec();
}
