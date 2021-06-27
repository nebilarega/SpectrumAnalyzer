#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <iostream>
#include "unistd.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Configure plot choosing combobox */
    ui->gTypeCBox->addItem("Spectrum");
    ui->gTypeCBox->addItem("Constellation");
    ui->gTypeCBox->addItem("Spectrogram");

    connect(ui->gTypeCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlotSelected(int)));

    MainWindow::configure_custom_plot();
    reader = new AsyncReader();
    reader->configure_rtl_sdr();
    reader->start();
    typedef QVector<double> PSD_vector;
    typedef QVector<double> IQ_vector;
    qRegisterMetaType<PSD_vector>("PSD_vector");
    qRegisterMetaType<IQ_vector>("IQ_vector");
    connect(&AsyncReader::instance(), SIGNAL(setPSD(PSD_vector)), this, SLOT(onPSDreceived(PSD_vector)));
    connect(&AsyncReader::instance(), SIGNAL(setIQ(IQ_vector)), this, SLOT(onIQreceived(IQ_vector)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onPSDreceived(QVector<double> receivedPSD){
    PSD = receivedPSD;
}
void MainWindow::onIQreceived(QVector<double> receivedIQ){
    IQ_val = receivedIQ;
}
void MainWindow::configure_custom_plot(){

    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->graph(0)->rescaleAxes();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
//    spectrumTimer.start(270); // Interval 0 means to refresh as fast as possible
    MainWindow::spectrumHandler();

    connect(&spectrumTimer,&QTimer::timeout, this, &MainWindow::oneSidedPSDfft);
    connect(&spectrogramTimer, &QTimer::timeout, this, &MainWindow::spectrogramPlot);
    connect(&constellationTimer,&QTimer::timeout, this, &MainWindow::constellation);
    std::cout << "Timer Has started " << std::endl;
}
void MainWindow::spectrumHandler(){
/* Disable other timers*/
    spectrogramTimer.stop();
    constellationTimer.stop();
/* Start own timer*/
    spectrumTimer.start(270);
}
void MainWindow::spectrogramHandler(){
/* Disable other timers*/
    spectrumTimer.stop();
    constellationTimer.stop();
/* Start own timer*/
    spectrogramTimer.start(270);
}
void MainWindow::constellationHandler(){
/* Disable other timers*/
    spectrogramTimer.stop();
    spectrumTimer.stop();
/* Start own timer*/
    constellationTimer.start(270);
}
void MainWindow::onPlotSelected(int index){
    if(index == 0)
        MainWindow::spectrumHandler();
    else if(index == 1)
        MainWindow::constellationHandler();
    else
        MainWindow::spectrogramHandler();
}
void MainWindow::twoSidedPSDfft(){
    double x_center = AsyncReader::getFrequency()/1000000-0.3;
    double x_bottom = x_center - 1;
    double x_top = x_center + 1;

      // add data to lines:
    for(int j=0; j < PSD.size(); j++){
        ui->customPlot->graph(0)->addData(j, PSD[j]-100);
    }

    // rescale value (vertical) axis to fit the current data:
    ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(x_bottom, x_top);
    ui->customPlot->yAxis->setRange(-80, 40);
    ui->customPlot->replot();
    ui->customPlot->graph(0)->data().data()->clear();


}
void MainWindow::oneSidedPSDfft(){
    int middleValue = PSD.size()/2;
    double x_bottom = AsyncReader::getFrequency()/1000000-0.3;
    double x_top = x_bottom + 1;

    for(int j=0; j < PSD.size()/2; j++){
        double x_val  = x_bottom + (increment*j);
        ui->customPlot->graph(0)->addData(x_val , PSD[middleValue+j]-80);
    }
    // get the range of the frequency inputs


    // rescale value (vertical) axis to fit the current data:
    ui->customPlot->graph(0)->rescaleValueAxis();

    ui->customPlot->xAxis->setRange(x_bottom, x_top);
//    ui->customPlot->xAxis->scaleRange(2);
    ui->customPlot->yAxis->setRange(-85, 0);
    ui->customPlot->xAxis->setLabel("f(MHz)");
    ui->customPlot->yAxis->setLabel("dBFS");
    ui->customPlot->replot();
    ui->customPlot->graph(0)->data().data()->clear();

}
void MainWindow::constellation(){
    if(IQ_val.size() == 1){
        return;
    }
    for(int j=0; j < IQ_val.size(); j+=2){
        ui->customPlot->graph(0)->addData(IQ_val[j], IQ_val[j+1]);
    }


   /* ui->customPlot->xAxis2->setVisible(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(false);
    ui->customPlot->yAxis2->setTickLabels(false)*/;
//    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    ui->customPlot->xAxis->setRange(-1, 1);
    ui->customPlot->yAxis->setRange(-1, 1);
//    ui->customPlot->xAxis->rescale(true);
//    ui->customPlot->yAxis->rescale(true);

    ui->customPlot->replot();
    ui->customPlot->graph(0)->data().data()->clear();
    IQ_val.clear();

}
void MainWindow::spectrogramPlot(){

    if(PSD.size() == 1){
        return;
    }
    if(PSD_buffer.size() < ny){
         PSD_buffer.push_front(PSD);
     }
     else{
         PSD_buffer.pop_back();
         PSD_buffer.push_front(PSD);
    }
    double x_bottom = AsyncReader::getFrequency()/1000000-0.3;
    double x_top = x_bottom + 1;
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("f(MHz)");
    ui->customPlot->yAxis->setLabel("time(250ms)");

    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);

    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(x_bottom, x_top), QCPRange(0, 2));
//    colorMap->setInterpolate(false);
    for (int yIndex=0; yIndex<ny; yIndex++)
    {
      while(PSD.empty());
      for (int xIndex=0; xIndex<nx; xIndex++)
      {
//          double x_val  = x_bottom + (increment*xIndex);
          if (yIndex < PSD_buffer.size()){
              colorMap->data()->setCell(xIndex, yIndex, PSD_buffer[yIndex][xIndex+4094]);

          }
          else
              colorMap->data()->setCell(xIndex, yIndex, -100);
      }
    }
    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Spectrogram");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
//    colorMap->rescaleDataRange();
    colorMap->setDataRange(QCPRange(-100,20));

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}
