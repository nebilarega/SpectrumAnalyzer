#ifndef INPUTFORM_H
#define INPUTFORM_H

#include <QWidget>

namespace Ui {
class InputForm;
}

class InputForm : public QWidget
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent*) override;
public:
    explicit InputForm(QWidget *parent = nullptr);
    ~InputForm();
public:
    void setClosed(int);
    int getClosed();
private:
    Ui::InputForm *ui;
    int closedInput = 0;
};

#endif // INPUTFORM_H
