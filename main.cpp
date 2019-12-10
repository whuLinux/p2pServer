#include <QCoreApplication>

#include "server.h"

/**
 * @brief main.cpp
 * @author 余宗宪
 * 程序入口
 */

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int tryTime = 0;

    Server * server = new Server();
    while(!server->startWorking() && tryTime < 100) {
        tryTime++;
    }

    return a.exec();
}
