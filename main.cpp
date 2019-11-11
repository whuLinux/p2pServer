#include <QCoreApplication>

#include "server.h"

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
