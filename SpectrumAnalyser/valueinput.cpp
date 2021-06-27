#include "valueinput.h"
#include "ui_valueinput.h"
#include "asyncreader.h"
#include <iostream>
#include <QCheckBox>

ValueInput::ValueInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueInput)
{
    ui->setupUi(this);
    ValueInput::setUp();
}

ValueInput::~ValueInput()
{
    delete ui;
}
void ValueInput::setUp(){
   double available_gains[] = {0.0, 0.9, 1.4, 2.7, 3.7, 7.7, 8.7, 12.5,
                                14.4, 15.7, 16.6, 19.7, 20.7, 22.9, 25.4,
                                28.0, 29.7, 32.8, 33.8, 36.4, 37.2, 38.6, 40.2, 42.1, 43};
/* Setting up the gains */
    for(int i=0; i < int(sizeof (available_gains)/ sizeof(double)); i++){
        std::cout << available_gains[i];
        ui->gainCombo->addItem(QString::number(available_gains[i]));
    }
    connect(ui->autoCheck, &QCheckBox::toggled, ui->gainCombo, &QComboBox::setDisabled);

/* Setting up spectrum */
    ui->specCombo->addItem("Two Sided");
    ui->specCombo->addItem("One sided");
/* set up plot and clear */
    ui->plot->setEnabled(false);
    connect(ui->clearValues, &QPushButton::clicked, this, &ValueInput::clearInputs);
    connect(ui->plot, &QPushButton::clicked, this, &ValueInput::plot);
    connect(ui->f_centerIn, &QLineEdit::textChanged, this, &ValueInput::frequencyValidator);
/* setup frequency editor */
    ui->f_centerIn->setValidator(new QDoubleValidator(24.0, 1500.0,2, ui->f_centerIn));
}

void ValueInput::frequencyValidator(QString input){
    double value = ui->f_centerIn->text().toDouble();
    if(value >= 24 && value <= 1500)
        ui->plot->setEnabled(true);
    else
        ui->plot->setEnabled(false);

}
void ValueInput::clearInputs(){
    ui->f_centerIn->clear();
    ui->autoCheck->setChecked(false);
    ui->gainCombo->setCurrentIndex(0);
    ui->specCombo->setCurrentIndex(0);
}
void ValueInput::plot(){
    int inputFrequecy = ui->f_centerIn->text().toDouble();
    if(ui->autoCheck->isChecked()){
        AsyncReader::setGain(0);
    }
    int inputGain = ui->gainCombo->currentText().toInt();
    AsyncReader::setFrequency(int(inputFrequecy*1000000));
    AsyncReader::setGain(inputGain);
    ValueInput::close();
}
