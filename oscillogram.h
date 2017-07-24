#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include <QWidget>
#include <QAudioBuffer>
#include <QtCharts/QChart>
#include <QMediaPlayer>
#include <QPair>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QHBoxLayout>

class Oscillogram : public QWidget
{
    Q_OBJECT
public:
    explicit Oscillogram(QWidget *parent = nullptr);
    void drawCharts();
    void setDuration(qint64 duration);
    void setChannelCount(int channelCount);

signals:

public slots:
    void processBuffer(const QAudioBuffer &buffer);

private:
    QVector<QPair<QtCharts::QChart *, QtCharts::QChartView *>> charts_;
    QVector<QtCharts::QSplineSeries *> series_;
    qint64 duration_;
    int channelCount_;
    bool isWindowsOS_;
};

#endif // OSCILLOGRAM_H
