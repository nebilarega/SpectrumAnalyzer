#ifndef ASYNCREADER_H
#define ASYNCREADER_H

#include "qcustomplot.h"
#include <QString>
#include <limits.h>
#include <rtl-sdr.h>

#define NUM_READ 16380

const int n_read = NUM_READ; /* Sample count & data points & FFT size */
static rtlsdr_dev_t *dev;
static int n,
    read_count = 0, /* Current read count */
    out_r, out_i, /* Real and imaginary parts of FFT *out values */
    _dev_id = 0, /* [ARG] RTL-SDR device ID (optional) */
    _samp_rate = 2048000, /* [ARG] Sample rate (optional) */
//    _gain = 14.4, /* [ARG] Device gain (optional) */
/*Permitable gains
 * 0.0 0.9 1.4 2.7 3.7 7.7 8.7 12.5 14.4 15.7 16.6 19.7 20.7 22.9 25.4 28.0 29.7 32.8 33.8 36.4 37.2 38.6 40.2 42.1 43
*/
    _refresh_rate = 500, /* [ARG] Refresh interval for continuous read (optional) */
    _num_read = INT_MAX, /* [ARG] Number of reads, set to max val of int (optional) */
    _cont_read = 0, /*[ARG] Continuously read samples from device (optional) */
    _mag_graph = 0, /*[ARG] Show magnitude instead of dB (optional) */
    _offset_tuning = 1;
static QVector<double> PSD_P;
static QVector<double> IQ;
class AsyncReader: public QThread
{
    Q_OBJECT

public:
    explicit AsyncReader(QObject *parent = 0);
    void run() override;
    static bool stop;
    QVector<double> getPSD();
private:
    static int& _center_freq(){
        static int _center_freq = 0;
        return _center_freq;
    }
    static int& _gain(){
        static int _gain = 0;
        return _gain;
    }
public slots:
//    void startWork();
    static QVector<double> createFFT(int, u_int8_t*);
    int configure_rtl_sdr();
    static void async_read_operation(unsigned char*, uint32_t, void *);
    static void do_exit();
    static AsyncReader& instance(){
        static AsyncReader a_instance;
        return a_instance;
    }
    static void setFrequency(const int);
    static int getFrequency();
    static void setGain(const int);
    static int getGain();
signals:
    void setIQ(QVector<double>);
    void setPSD(QVector<double>);
};

#endif // ASYNCREADER_H
