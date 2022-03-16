#include "./ui_MainWindow.h"
#include "MainWindow.hpp"
#include "widgets/ModeSelect.hpp"
#include "widgets/AppWidget.hpp"
#include <iostream>


MainWindow::MainWindow(QWidget* parent)
    :   QMainWindow(parent),
        ui_(new Ui::MainWindow())
{
    ui_->setupUi(this);

    auto const selectMode = new ModeSelect(this);
    setCentralWidget(selectMode);
    setFixedSize(selectMode->size());

    // If a selected signal is emitted, replace the current widget
    connect(selectMode, &ModeSelect::Selected, this, [this](Mode mode){
        auto const appWidget = new AppWidget(mode, this);
        setCentralWidget(appWidget);
        setFixedSize(appWidget->size());
    });
}

MainWindow::~MainWindow() {
    delete ui_;
}