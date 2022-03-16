#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <memory>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    Q_OBJECT
    Ui::MainWindow* ui_;
};

#endif