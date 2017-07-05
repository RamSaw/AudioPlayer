#include <QFileDialog>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QStandardPaths>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "oscillogramwidget.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    audioOscillogram = new OscillogramWidget(this);
    audioOscillogram->setMinimumSize(this->width(), this->height() * 0.1);
    audioOscillogram->move(0, this->height() - this->height() * 0.1);
    audioOscillogram->show();

    audioProbe = new QAudioProbe(this);
    connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), audioOscillogram, SLOT(processBuffer(QAudioBuffer)));
    audioProbe->setSource(&audioPlayer_.player_);
}

MainWindow::~MainWindow() {
    delete ui;
    delete audioProbe;
    delete audioOscillogram;
}

void MainWindow::on_chooseVideoButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose video"),
        QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()),
        tr("Video Files (*.avi *.mkv *.mp4 *.mp3 *.flac)"));
    if (!fileName.isEmpty()) {
        ui->filenameLabel->setText(QFileInfo(fileName).fileName());
        qDebug() << fileName;
        audioPlayer_.setFile(fileName);
    }
}

void MainWindow::on_startButton_clicked() {
    audioPlayer_.play();
}

void MainWindow::on_stopButton_clicked()
{
    audioPlayer_.stop();
}

void MainWindow::on_saveButton_clicked()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,
        tr("Select path to saving file"), "/home/");
    if (!saveFileName.isEmpty()) {
        qDebug() << saveFileName;
        audioPlayer_.saveInOtherThread(saveFileName);
    }
}
