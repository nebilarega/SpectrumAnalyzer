#include "asyncreader.h"
#include <QVector>
#include <QTextStream>
#include <string>
#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <unistd.h>

#define REAL 0
#define IMAG 1

AsyncReader::AsyncReader(QObject* parent): QThread(parent)
{

}
void AsyncReader::run(){
    rtlsdr_read_async(dev, async_read_operation, NULL,0,n_read*16);
}
QVector<double> AsyncReader::createFFT(int sampleCount, u_int8_t* buf){
    QVector<double> PSD;
    fftw_complex* x = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*sampleCount);
    fftw_complex* y = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*sampleCount);


    int n=0;
    double pi = 3.1415926;
    for(int i=0; i<sampleCount; i+=2){
        x[n][REAL] = (double(buf[i]) - 127.34) / 255;
        x[n][IMAG] = (double(buf[n+1]) - 127.34) / 255;

//        blackman windowing
        double blackman = 0.42 - 0.5 * cos(2*pi*i / (sampleCount/2 - 1)) + 0.08 * cos(4*pi*i / (sampleCount/2 - 1));
        x[n][REAL] = x[n][REAL] * blackman;
        x[n][IMAG] = x[n][IMAG] * blackman;
        n++;
    }
    fftw_plan plan = fftw_plan_dft_1d(sampleCount, x, y, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(plan);
//    std::cout << "the code breaks after execute" << std::endl;
    fftw_destroy_plan(plan);
    fftw_cleanup();

    // calculate the psd as only the magnitude

    for(int i=0; i<sampleCount/2; i++){
        double intermidate = sqrt(y[i][REAL]*y[i][REAL] + y[i][IMAG]*y[i][IMAG]);
//        std::cout << "real : " << y[i][REAL] << " img : " << y[i][IMAG]  << std::endl;
        PSD.push_back(10*log10(intermidate));
    }

    return PSD;
}

int AsyncReader::configure_rtl_sdr(){
    static int center_freq = getFrequency();
    static int gain = getGain();
    int device_count = rtlsdr_get_device_count();
    if (!device_count){
        std::cout << "No supported devices found "<< std::endl;
        exit(1);
    }
    std::cout << "Found " << device_count << " " << std::endl;
    int dev_open = rtlsdr_open(&dev, 0);
    if(dev_open < 0){
        std::cout << "Failed to open device " << std::endl;
        exit(1);
    }
    else{
        std::cout << "Using device: " << dev_open << std::endl;
    }
    if(!gain){
        rtlsdr_set_tuner_gain_mode(dev, gain);
    }
    else{
        rtlsdr_set_tuner_gain_mode(dev, 1);
        int gain_count = rtlsdr_get_tuner_gains(dev, NULL);
        // log_info("Supported gain values (%d): ", gain_count);
        int gains[gain_count], supported_gains = rtlsdr_get_tuner_gains(dev, gains);
        for (int i = 0; i < supported_gains; i++){
            /**!
             * Different RTL-SDR devices have different supported gain
             * values. So select gain value between 1.0 and 3.0
             */
            if (gains[i] > 45 && gains[i] < 100)
                gain = gains[i];
        }
        rtlsdr_set_tuner_gain(dev, gain);
    }
    // rtlsdr_set_offset_tuning(dev, _offset_tuning);

    rtlsdr_set_offset_tuning(dev, 1);
    rtlsdr_set_center_freq(dev, center_freq);
    rtlsdr_set_sample_rate(dev, _samp_rate);
    int r = rtlsdr_reset_buffer(dev);
    if (r < 0){
        std::cout << "Failed to reset buffers.\n" << std::endl;
        return 1;
    }
    return 0;
}
void AsyncReader::async_read_operation(unsigned char *n_buf, uint32_t len, void *ctx){
    PSD_P = createFFT(n_read, n_buf);
    for(int i=0; i<n_read; i++){
        IQ.push_back((double(n_buf[i]) - 127.34) / 255);
    }
    emit instance().setPSD(PSD_P);
    emit instance().setIQ(IQ);
    IQ.clear();
    usleep(_refresh_rate*500);
    rtlsdr_read_async(dev, async_read_operation, NULL, 0, n_read * 16);
}
void AsyncReader::do_exit(){
    rtlsdr_cancel_async(dev);
}
QVector<double> AsyncReader::getPSD(){
    return PSD_P;
}
void AsyncReader::setFrequency(const int center_freq){
    _center_freq() = center_freq;
}
int AsyncReader::getFrequency(){
    return _center_freq();
}
void AsyncReader::setGain(const int gain){
    _gain() = gain;
}
int AsyncReader::getGain(){
    return _gain();
}
