/**
 * @file AppWidget.cpp
 * @brief Implements the AppWidget UI class
 * @author Noak Palander
 * @version 1.0
 * @see AppWidget.hpp
 */

#include "AppWidget.hpp"
#include "./ui_AppWidget.h"
#include <QMessageBox>
#include <QLineEdit>
#include <iostream>
#include <memory>
#include "../../core/Misc.hpp"


AppWidget::AppWidget(Chat::Mode mode, QWidget* parent)
    :   QWidget(parent),
        ui_(new Ui::AppWidget()),
        mode_(mode),
        processor_(nullptr)
{
    // Creates the UI
    ui_->setupUi(this);
    setFixedSize(size());

    // Updates some texts based on the chat mode
    ui_->startBtn->setText(mode_ == Chat::Mode::Server ? "Start" : "Connect");
    ui_->consoleLabel->setText(Misc::QFormat("{} console", mode_));

    if (mode_ == Chat::Mode::Server)
        ui_->addrEdit->setDisabled(true);

    // If the start/connect button was pressed
    connect(ui_->startBtn, &QPushButton::pressed, this, [this]{
        switch(mode_) {
            // Server mode
            case Chat::Mode::Server:
                try {
                    // Constructs a processor in server mode
                    processor_ = std::make_unique<Chat::Processor>(ui_->portEdit->text().toInt(),
                                                                   std::bind_front(&AppWidget::Received, this),
                                                                   std::bind_front(&AppWidget::Connected, this),
                                                                   std::bind_front(&AppWidget::Disconnected, this));
                    ui_->startBtn->setDisabled(true);
                }
                catch(asio::system_error& e) {
                    QMessageBox::critical(this, "Failed to start the server!",
                                          Misc::QFormat("asio::system_error thrown, reason being: {}", e.what()));
                }
                break;

            // Client mode
            case Chat::Mode::Client:
                try {
                    // Constructs a processor in client mode
                    processor_ = std::make_unique<Chat::Processor>(ui_->portEdit->text().toInt(),
                                                                   ui_->addrEdit->text().toStdString(),
                                                                   std::bind_front(&AppWidget::Received, this),
                                                                   std::bind_front(&AppWidget::Connected, this),
                                                                   std::bind_front(&AppWidget::Disconnected, this));
                }
                catch(asio::system_error& e) {
                    QMessageBox::critical(this, "Failed to connect to the server!",
                                          Misc::QFormat("asio::system_error thrown, reason being: {}", e.what()));
                }
                break;
        }
    });

    // If a message was attempted to be sent
    connect(ui_->lineEdit, &QLineEdit::returnPressed, this, [this]{
        QString const text = ui_->lineEdit->text();

        if (!text.isEmpty()) {
            // Constructs a new message given the written text
            auto const message = Chat::Message::From(text.toStdString());

            // The item that will be displayed on the local chat box
            auto listItem = new QListWidgetItem(ui_->chatBox);
            listItem->setText(Misc::QFormat("[You]: {}", message.Contents()));
            ui_->lineEdit->clear();

            // Stores the message's hash to the timestamp and item, so we can go back using the ID to update the text to also
            // show the response time
            data_.emplace(message.Identifier(), std::pair{ message.Timestamp(), listItem });

            // Transmit message
            Misc::Debug("[{}]: Sent a message with ID {}!\n", mode_, message.Identifier());
            processor_->Transmit(message);
        }
    });

    // Updates received text onto the chatbox
    connect(this, &AppWidget::Append, this, [this](QString const& data) {
        auto listItem = new QListWidgetItem(ui_->chatBox);
        listItem->setText(data.trimmed());
        ui_->chatBox->addItem(listItem);
    });

    // Writes some text to the console box
    connect(this, &AppWidget::Log, this, [this](QString const& text) {
        ui_->console->insertPlainText(text);
    });

    connect(this, &AppWidget::NoHost, this, [this]{
        processor_.reset(nullptr);
    });
}


AppWidget::~AppWidget() {
    delete ui_;
}

/**
 * @brief The callback is invoked when a client connected (server mode), or when we connect to the server (client mode)
 */
void AppWidget::Received(Chat::Message const& message) {
    // Received a new message
    if (message.Type() == Chat::MessageType::New) {
        emit Append(Misc::QFormat("[{}]: {}", !mode_, message.Contents()));
    }
    else {
        Misc::Debug("[{}]: Received a message with ID {}!\n", mode_, message.Identifier());
        // Finds the related message that was recently acknowledged
        if (auto iter = data_.find(message.Identifier()); iter != data_.end()) {
            auto[time, widget] = iter->second;

            // Calculate the response time
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(message.Timestamp() - time);

            // Updates the text
            widget->setText(Misc::QFormat("{} \t\t[Delivered in {} us]",
                                          widget->text().trimmed().toStdString(), duration.count()));

            // Remove the stored data as it's no longer necessary
            data_.erase(iter);
        }
    }
}

/**
 * @brief The callback is invoked when a client connected (server mode), or when we connect to the server (client mode)
 */
void AppWidget::Connected() {
    emit Log(Misc::QFormat("Established a connection with {}\n", !mode_));
    ui_->lineEdit->setEnabled(true);

    if (mode_ == Chat::Mode::Client)
        ui_->startBtn->setDisabled(true);
}

/**
 * @brief The callback is invoked when a client disconnects (server mode), or when we disconnect (client mode)
 */
void AppWidget::Disconnected() {
    if (mode_ == Chat::Mode::Server) {
        emit Log(Misc::QFormat("{} disconnected, awaiting a new connection\n", !mode_));
    }
    else {
        emit NoHost();
        emit Log("Cannot detect a server, please start the server and then try to connect!\n");
        ui_->startBtn->setEnabled(true);
    }

    ui_->lineEdit->setDisabled(true);
}