#ifndef OSCILLOGRAMWIDGET_H
#define OSCILLOGRAMWIDGET_H

#include <QThread>
#include <QVideoFrame>
#include <QAudioBuffer>
#include <QWidget>

class QAudioLevel;

class OscillogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OscillogramWidget(QWidget *parent = 0);
    ~OscillogramWidget();
    void setLevels(int levels) { m_levels = levels; }

public slots:
    void processBuffer(QAudioBuffer buffer);
    void setOscillogram(QVector<qreal> oscillogram);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<qreal> m_oscillogram;
    int m_levels;
    bool m_isBusy;
    QVector<QAudioLevel *> audioLevels;


};

#endif // OSCILLOGRAMWIDGET_H
