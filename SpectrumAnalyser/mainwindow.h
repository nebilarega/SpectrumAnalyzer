#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "asyncreader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static QTimer spectrumTimer;
static QTimer spectrogramTimer;
static QTimer constellationTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
public:
    QVector<double> PSD{0};
    QVector<QVector<double>> PSD_buffer;
    QVector<double> IQ_val{0};
    double increment = 0.0002441406;
public slots:
    void configure_custom_plot();
    void twoSidedPSDfft();
    void oneSidedPSDfft();
    void constellation();
    void spectrogramPlot();
//    void spectrogram();
    void onIQreceived(QVector<double>);
    void onPSDreceived(QVector<double>);
    void spectrumHandler();
    void spectrogramHandler();
    void constellationHandler();
    void onPlotSelected(int index);

public:
     void startWork();
     AsyncReader *reader;
     int nx = 4096;
     int ny = 20;  // this will set the max buffer size

signals:
    void dataSetSignal(QVector<double>);

};
#endif // MAINWINDOW_H
