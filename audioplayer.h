#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QString>
#include <QMediaPlayer>
#include <QAudioBuffer>
#include <thread>
#include <atomic>

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    QMediaPlayer player_;

    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();
    void setFile(const QString &fileName);
    void play();
    void stop();
    void save(const QString &saveFileName);
    void saveInOtherThread(const QString &saveFileName);

signals:

public slots:

private:
    QString fileName_;
    std::thread saveThread_;
    std::atomic<bool> isSaveFinished_;
};

#endif // AUDIOPLAYER_H
