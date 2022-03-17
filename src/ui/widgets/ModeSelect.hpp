/**
 * @file ModeSelect.hpp
 * @brief Contains the declaration for the ModeSelect class, whoes only purpose is to select the mode (Server/Client)
 * @author Noak Palander
 * @version 1.0
 */

#ifndef MODE_SELECT_HPP
#define MODE_SELECT_HPP

#include <QWidget>
#include "../../core/Mode.hpp"

namespace Ui {
    class ModeSelect;
}

/**
 * @class ModeSelect
 * @brief Partially implemented by Qt's UIC, selects server/client given user choice
 */
class ModeSelect : public QWidget {
public:
    explicit ModeSelect(QWidget* parent = nullptr);
    ~ModeSelect() override;

    /**
     * @brief Invoked internally when a mode is selected
     * @attention This is not a normal function, it has no implementation, it's a Qt signal
     * @param mode the mode to start
     */
    Q_SIGNAL void Selected(Chat::Mode mode);

private:
    Q_OBJECT
    Ui::ModeSelect* ui_;    /**< Qt doesn't handle RAII well with UI's, this is an owning pointer, and should be the only owner. */
};

#endif // !MODE_SELECT_HPP
