#include "inputform.h"
#include "ui_inputform.h"
#include "mainwindow.h"

InputForm::InputForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputForm)
{
    ui->setupUi(this);
}

InputForm::~InputForm()
{
    delete ui;
}
void InputForm::closeEvent(QCloseEvent *event){
    event->ignore();
    InputForm::setClosed(1);
    event->accept();
}
void InputForm::setClosed(int closedInput){
    this->closedInput = closedInput;
}
int InputForm::getClosed(){
    return this->closedInput;
}
