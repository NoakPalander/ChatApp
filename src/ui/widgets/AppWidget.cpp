#include "AppWidget.hpp"
#include "./ui_AppWidget.h"
#include <QMessageBox>
#include <QLineEdit>
#include <iostream>
#include <memory>
#include "../../core/Misc.hpp"


AppWidget::AppWidget(Mode mode, QWidget* parent)
        :   QWidget(parent),
            ui_(new Ui::AppWidget()),
            mode_(mode)
{
    ui_->setupUi(this);
    setFixedSize(size());

    ui_->startBtn->setText(mode_ == Mode::Server ? "Start" : "Connect");
    ui_->consoleLabel->setText(Misc::QFormat("{} console", mode_));

    // If the start/connect button was pressed
    connect(ui_->startBtn, &QPushButton::pressed, this, [this]{
        switch(mode_) {
            // Server mode
            case Mode::Server:
                try {
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
            case Mode::Client:
                try {
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
            // Write the message to the (local) chatbox
            auto const message = Chat::Message::From(text.toStdString());
            auto listItem = new QListWidgetItem(ui_->chatBox);
            listItem->setText(Misc::QFormat("[You]: {}", message.Contents()));
            ui_->lineEdit->clear();
            data_.emplace(message.Identifier(), std::pair{ message.Timestamp(), listItem });

            // Transmit message
            Misc::Debug("[{}]: Sent a message with ID {}!\n", mode_ == Mode::Client ? "Client" : "Server", message.Identifier());
            processor_->Transmit(message);
        }
    });

    // Updates received text onto the chatbox
    connect(this, &AppWidget::Append, this, [this](QString const& data){
        auto listItem = new QListWidgetItem(ui_->chatBox);
        listItem->setText(data.trimmed());
        ui_->chatBox->addItem(listItem);
    });

    // Display some information in a message popup
    connect(this, &AppWidget::InfoBox, this, [this](QString const& title, QString const& text) {
        QMessageBox::information(this, title, text);
    });

    // Displays a critical error in a message popup
    connect(this, &AppWidget::ErrorBox, this, [this](QString const& title, QString const& text) {
        QMessageBox::critical(this, title, text);
    });

    connect(this, &AppWidget::Log, this, [this](QString const& text) {
        ui_->console->insertPlainText(text);
    });
}


AppWidget::~AppWidget() {
    delete ui_;
}

void AppWidget::Received(Chat::Message const& message) {
    // Received a new message
    if (message.Type() == Chat::MessageType::New) {
        emit Append(Misc::QFormat("[{}]: {}", !mode_, message.Contents()));
    }
    else {
        Misc::Debug("[{}]: Received a message with ID {}!\n", mode_, message.Identifier());

        // Retreive the previously sent message (that we received a response to)'s timestamp and corresponding widget
        auto[time, widget] = data_.at(message.Identifier());

        // Calculate the response time
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(message.Timestamp() - time);

        // Updates the text
        widget->setText(Misc::QFormat("{} \t\t[Delivered in {} us]",
                                      widget->text().trimmed().toStdString(), duration.count()));
    }
}

void AppWidget::Connected() {
    emit Log(Misc::QFormat("Established a connection with {}\n", !mode_));
    ui_->lineEdit->setEnabled(true);

    if (mode_ == Mode::Client)
        ui_->startBtn->setDisabled(true);
}

// When a client disconnects from the server
void AppWidget::Disconnected() {
    if (mode_ == Mode::Server) {
        emit Log(Misc::QFormat("{} disconnected, awaiting a new connection\n", !mode_));
    }
    else {
        emit Log(Misc::QFormat("{} disconnected, click connect once the server is up again\n", !mode_));
        ui_->startBtn->setEnabled(true);
    }

    ui_->lineEdit->setDisabled(true); // can't send messages if there is no connection
}