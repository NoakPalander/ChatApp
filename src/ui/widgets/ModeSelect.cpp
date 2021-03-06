/**
 * @file ModeSelect.cpp
 * @brief Implements the ModeSelect UI class
 * @author Noak Palander
 * @version 1.0
 * @see ModeSelect.hpp
 */

#include "ModeSelect.hpp"
#include "ui_ModeSelect.h"


ModeSelect::ModeSelect(QWidget* parent)
    :   QWidget(parent),
        ui_(new Ui::ModeSelect) {

    ui_->setupUi(this);
    setFixedSize(size());

    // Emit a signal with 'Mode::Client' if the client button is pressed
    connect(ui_->clientBtn, &QPushButton::pressed, this, [this]{
        emit Selected(Chat::Mode::Client);
    });

    // Emit a signal with 'Mode::Server' if the server button is pressed
    connect(ui_->serverBtn, &QPushButton::pressed, this, [this]{
        emit Selected(Chat::Mode::Server);
    });
}

ModeSelect::~ModeSelect() {
    delete ui_;
}
