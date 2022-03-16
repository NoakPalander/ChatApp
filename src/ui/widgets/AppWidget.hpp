#ifndef CHATAPP_APPWIDGET_HPP
#define CHATAPP_APPWIDGET_HPP

#include "../../core/Mode.hpp"
#include "../../core/Processor.hpp"
#include "../../core/Message.hpp"
#include <QWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <thread>
#include <variant>
#include <utility>
#include <memory>

namespace Ui {
    class AppWidget;
}

class AppWidget : public QWidget {
public:
    explicit AppWidget(Mode mode, QWidget* parent = nullptr);
    ~AppWidget() override;

    Q_SIGNAL void Append(QString const&);
    Q_SIGNAL void InfoBox(QString const& title, QString const& text);
    Q_SIGNAL void ErrorBox(QString const& title, QString const& text);
    Q_SIGNAL void Log(QString const& text);


private:
    void Received(Chat::Message const& message);
    void Connected();
    void Disconnected();

private:
    Q_OBJECT
    Ui::AppWidget* ui_;
    Mode mode_;
    std::unordered_map<Chat::Message::HashType, std::pair<std::chrono::system_clock::time_point, QListWidgetItem*>> data_;
    std::unique_ptr<Chat::Processor> processor_;
};

#endif // CHATAPP_APPWIDGET_HPP