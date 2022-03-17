/**
 * @file MainWindow.hpp
 * @brief Contains the declaration for the MainWindow class, which acts as the UI main, it manages the other widgets
 * @author Noak Palander
 * @version 1.0
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

/**
 * @class MainWindow
 * @brief Manages other widgets internally, it's partially implemented by Qt's UIC
 * @author Noak Palander
 */
class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief When the main window is destructed, the application qutis.
     */
    ~MainWindow() override;

private:
    Q_OBJECT
    Ui::MainWindow* ui_;    /**< Qt doesn't handle RAII well with UI, this should be the only owner. */
};

#endif