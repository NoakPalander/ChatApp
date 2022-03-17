/**
 * @file MainWindow.cpp
 * @brief Contains the implementation for the MainWindow UI class
 * @author Noak Palander
 * @version 1.0
 * @see MainWindow.hpp
 */

#include "./ui_MainWindow.h"
#include "MainWindow.hpp"
#include "widgets/ModeSelect.hpp"
#include "widgets/AppWidget.hpp"


MainWindow::MainWindow(QWidget* parent)
    :   QMainWindow(parent),
        ui_(new Ui::MainWindow())
{
    ui_->setupUi(this);

    // Sets the main widget as the ModeSelect
    auto const selectMode = new ModeSelect(this);
    setCentralWidget(selectMode);
    setFixedSize(selectMode->size());

    // If a selected signal is emitted, replace the current widget
    connect(selectMode, &ModeSelect::Selected, this, [this](Chat::Mode mode){
        // Starts the AppWidget, passes along the selected mode
        auto const appWidget = new AppWidget(mode, this);
        setCentralWidget(appWidget);
        setFixedSize(appWidget->size());
    });
}

MainWindow::~MainWindow() {
    delete ui_;
}