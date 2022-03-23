/**
 * @file AppWidget.hpp
 * @brief Contains the declarations for the AppWidget class, which acts as the main application after the mode is selected.
 * @author Noak Palander
 * @version 1.0
 */

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

/**
 * @class AppWidget
 * @brief Partially implemented by Qt's UIC, starts the server/client and keep tracks of message IDs for response times.
 * @author Noak Palander
 */
class AppWidget : public QWidget {
public:
    explicit AppWidget(Chat::Mode mode, QWidget* parent = nullptr);

    /**
     * @brief When the destructor is run, the UI will close, ultimately closing the entire application.
     */
    ~AppWidget() override;

    /**
     * @brief Invoked internally when text wants to be appended to the chat box
     * @attention This is not a normal function, it has no implementation, it's a Qt signal
     * @param text the text that should be written to the chatbox
     *
     * It's mainly invoked from the server/client callbacks, however these are run on a separate thread managed by the processor's
     * io_service, thus we can't perform draws within the callbacks, hence we emit a signal so Qt can perform draws on the UI thread.
     */
    Q_SIGNAL void Append(QString const& text);

    /**
     * @brief Invoked internally when text wants to be written onto the console, as a log
     * @attention This is not a normal function, it has no implementation, it's a Qt signal
     * @param text the text that should be logged
     *
     * It's mainly invoked from the server/client callbacks, however these are run on a separate thread managed by the processor's
     * io_service, thus we can't perform draws within the callbacks, hence we emit a signal so Qt can perform draws on the UI thread.
     */
    Q_SIGNAL void Log(QString const& text);

    /**
     * @brief Invoked internally when no host/server is connected, this is a fatal error as the server should never go down.
     * @attention This is not a normal function, it has no implementation, it's a Qt signal
     */
    Q_SIGNAL void NoHost();

private:
    /**
     * @brief The callback is invoked when the processor receives a message
     * @param message the message we received from the client/server
     */
    void Received(Chat::Message const& message);

    /**
     * @brief The callback is invoked when a client connected (server mode), or when we connect to the server (client mode)
     */
    void Connected();

    /**
     * @brief The callback is invoked when a client disconnects (server mode), or when we disconnect (client mode)
     */
    void Disconnected();

private:
    Q_OBJECT
    Ui::AppWidget* ui_; /**< Qt doesn't handle RAII well with UI's.., this is an owning pointer */
    Chat::Mode mode_;
    std::unique_ptr<Chat::Processor> processor_;

    std::unordered_map<Chat::Message::HashType, std::pair<std::chrono::system_clock::time_point, QListWidgetItem*>> data_;
    /**< Contains a map of message hashes and their corresponding sent-time and the QListWidgetItem that is displayed on the chatbox
     * so we can update the contents to show the response time */
};

#endif // CHATAPP_APPWIDGET_HPP