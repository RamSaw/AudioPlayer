#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioProbe>
#include <QLabel>

#include "audioplayer.h"
#include "oscillogramwidget.h"

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

private:
    Ui::MainWindow *ui;

    AudioPlayer audioPlayer_;

    OscillogramWidget *audioOscillogram;
    QAudioProbe *audioProbe;
};

#endif // MAINWINDOW_H
