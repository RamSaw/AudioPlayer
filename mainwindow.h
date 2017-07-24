#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioProbe>
#include <QLabel>
#include <QProcess>
#include <QPushButton>

#include "oscillogram.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_chooseVideoButton_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_saveButton_clicked();

    void processStarted();

    void processReadyReadStandardOutput();

    void processFinished();

    void processErrorOccurred();

    void processMediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);

    void on_setFFmpegExe_clicked();

private:
    Ui::MainWindow *ui;

    /* Variables for playing and saving audio */
    QMediaPlayer *player_;
    QString fileName_;
    QString saveFileName_;
    QProcess *savingProcess_;
    QString ffmpegPath_;
    QString cmd_;
    QPushButton* pb;

    Oscillogram *oscillogram;
    QAudioProbe *audioProbe;

    void guessFFmpegPath();
};

#endif // MAINWINDOW_H
