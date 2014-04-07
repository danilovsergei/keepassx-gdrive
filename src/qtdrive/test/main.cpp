#include <QApplication>

#include "main_window.h"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    a.setOrganizationName("prog-org-ru-developers");
    a.setApplicationName("qtgoogledrive-test");
    a.setApplicationVersion("0.1");
    MainWindow wnd;
    wnd.show();
    return a.exec();
}
