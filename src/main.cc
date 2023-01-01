#include <QtGui>
#include <time.h>
#include "workSpace.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // Start aplikace
    QApplication app(argc, argv);
    WorkSpace workSpace;
    workSpace.show();

    return app.exec();
}
