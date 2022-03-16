#include <QApplication>
#include "ui/MainWindow.hpp"


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return QApplication::exec();
}