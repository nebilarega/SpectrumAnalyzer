#ifndef VALUEINPUT_H
#define VALUEINPUT_H

#include <QDialog>

namespace Ui {
class ValueInput;
}

class ValueInput : public QDialog
{
    Q_OBJECT

public:
    explicit ValueInput(QWidget *parent = nullptr);
    ~ValueInput();

private:
    Ui::ValueInput *ui;
public:
    void setUp();
    void clearInputs();
    void plot();
public slots:
    void frequencyValidator(QString);
private:

};

#endif // VALUEINPUT_H
