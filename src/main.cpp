#include <QApplication>
#include "ui/MainWindow.hpp"


int main(int argc, char** argv) {
    // Starts the UI
    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return QApplication::exec();
}