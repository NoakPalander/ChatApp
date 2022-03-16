#ifndef MODE_SELECT_HPP
#define MODE_SELECT_HPP

#include <QWidget>
#include "../../core/Mode.hpp"

namespace Ui {
    class ModeSelect;
}

class ModeSelect : public QWidget {
public:
    explicit ModeSelect(QWidget* parent = nullptr);
    ~ModeSelect() override;

    Q_SIGNAL void Selected(Mode mode);

private:
    Q_OBJECT
    Ui::ModeSelect* ui_;
};

#endif // !MODE_SELECT_HPP
