#include "oscillogram.h"

#include <QMainWindow>
#include <QCoreApplication>

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format) {
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

template <class T>
QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels) {
    QVector<qreal> values;
    values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qreal(buffer[i * channels + j]);
            if (qAbs(value) > qAbs(values[j]))
            values.replace(j, value);
        }
    }

    return values;
}

// returns the audio level for each channel
QVector<qreal> getYValues(const QAudioBuffer& buffer) {
    QVector<qreal> values;

    if (!buffer.isValid())
        return values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = (values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

Oscillogram::Oscillogram(QWidget *parent) : QWidget(parent), duration_(-1), channelCount_(0) {
}

void Oscillogram::drawCharts() {
    if (duration_ == -1 || channelCount_ == 0)
        return;

    for (int i = 0; i < charts_.size(); i++)
        delete charts_[i].first, delete charts_[i].second, delete series_[i];
    charts_.clear();
    series_.clear();

    for (int i = 0; i < channelCount_; i++) {
        //![1]
        QtCharts::QSplineSeries *series = new QtCharts::QSplineSeries();
        series->setName("spline");
        series_.append(series);
        //![1]

        //![3]
        QtCharts::QChart *chart = new QtCharts::QChart();
        chart->legend()->hide();
        chart->addSeries(series);
        chart->setTitle("Channel " + QString::number(i + 1));
        chart->createDefaultAxes();
        chart->axisY()->setRange(-1, 1);
        chart->axisX()->setRange(0, duration_ / 1000.0);
        //![3]

        //![4]
        QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        //![4]

        chartView->setFixedHeight(400);
        chartView->setFixedWidth(1200);
        chartView->show();

        charts_.append(qMakePair(chart, chartView));
    }
}

void Oscillogram::setDuration(qint64 duration) {
    duration_ = duration;
}

void Oscillogram::setChannelCount(int channelCount) {
    channelCount_ = channelCount;
}

void Oscillogram::processBuffer(const QAudioBuffer &buffer) {
    if (channelCount_ != buffer.format().channelCount()) {
        channelCount_ = buffer.format().channelCount();
        drawCharts();
    }

    if (duration_ != -1) {
        QVector<qreal> values = getYValues(buffer);
        qreal startTime = buffer.startTime();
        for (int j = 0; j < channelCount_; j++)
            series_[j]->append(startTime / 1000000.0, values[j]);
    }
}
