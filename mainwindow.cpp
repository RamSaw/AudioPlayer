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
#include <QMessageBox>
#include <QMediaMetaData>

#include "mainwindow.h"
#include "ui_mainwindow.h"

static void findRecursion(const QString &path, const QString &pattern, QString &result)
{
    if (result != "")
        return;

    QDir currentDir(path);
    const QString prefix = path + QLatin1Char('/');
    foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks)) {
           result = prefix + match;
           return;
    }
    foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
        findRecursion(prefix + dir, pattern, result);
}

void MainWindow::guessFFmpegPath() {
    QString osType = QSysInfo::productType();
    if (osType == "windows" || osType == "winrt") {
        findRecursion("C:/Program Files", "ffmpeg.exe", ffmpegPath_);
    }
    else {
        ffmpegPath_ = "ffmpeg";
    }

    if (ffmpegPath_ == "") {
        ffmpegPath_ = "ffmpeg";

        QMessageBox alert;
        alert.setText("Cannot locate ffmpeg.exe, path set to default command: ffmpeg. Be sure that path to ffmpeg.exe is set in PATH environment variable.");
        alert.exec();
    }

    ui->exeLabel->setText(ffmpegPath_);
    ffmpegPath_.append("\"");
    ffmpegPath_.insert(0, "\"");
}

void MainWindow::processMediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus) {
    if (mediaStatus == QMediaPlayer::LoadedMedia) {
        oscillogram->setDuration(player_->duration());
        int channelCount = player_->metaData(QMediaMetaData::ChannelCount).toInt();
        if (channelCount != 0)
            oscillogram->setChannelCount(channelCount);
        oscillogram->drawCharts();
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    player_ = new QMediaPlayer(this);
    savingProcess_ = new QProcess(this);

    oscillogram = new Oscillogram(this);

    connect(player_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(processMediaStatusChanged(QMediaPlayer::MediaStatus)));

    audioProbe = new QAudioProbe(this);
    connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), oscillogram, SLOT(processBuffer(QAudioBuffer)));
    audioProbe->setSource(player_);

    connect(savingProcess_, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(processErrorOccurred()));
    connect(savingProcess_, SIGNAL(started()), this, SLOT(processStarted()));
    connect(savingProcess_, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadyReadStandardOutput()));
    connect(savingProcess_, SIGNAL(finished(int)), this, SLOT(processFinished()));

    guessFFmpegPath();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_chooseVideoButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose video"),
        QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()),
        tr("Video Files (*.wmv *.avi *.mkv *.mp4);;All files (*.*)"));

    if (!fileName.isEmpty()) {
        ui->filenameLabel->setText(QFileInfo(fileName).fileName());
        fileName_ = fileName;
        player_->setMedia(QUrl::fromLocalFile(fileName));
    }
}

void MainWindow::on_startButton_clicked() {
    player_->play();
}

void MainWindow::on_stopButton_clicked() {
    player_->stop();
}

void MainWindow::processStarted() {
    ui->ffmpegOutput->append("Saving started");
}

void MainWindow::processReadyReadStandardOutput() {
    ui->ffmpegOutput->append(savingProcess_->readAllStandardOutput());
}

void MainWindow::processFinished() {
    ui->ffmpegOutput->append("Finished");

    if (QFile::exists(saveFileName_))
        ui->ffmpegOutput->append("Success. Audio file created.");
    else
        ui->ffmpegOutput->append("Failed. Something went wrong. No audio file.");
}

void MainWindow::processErrorOccurred() {
    ui->ffmpegOutput->append(savingProcess_->errorString());
    ui->ffmpegOutput->append("Program tried to run command: " + cmd_);
}

void MainWindow::on_saveButton_clicked() {
    QString saveFileName = QFileDialog::getSaveFileName(this,
        tr("Select path to saving file"),
        QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()),
        tr("Audio File (*.mp3);;All files (*.*)"));

    if (!saveFileName.isEmpty()) {
        saveFileName_ = saveFileName;

        /* Saving */
        QString addedQuetesFileName = fileName_, addedQuetesSaveFileName = saveFileName_;
        addedQuetesFileName.append("\"");
        addedQuetesFileName.insert(0, "\"");
        addedQuetesSaveFileName.append("\"");
        addedQuetesSaveFileName.insert(0, "\"");

        cmd_ = ffmpegPath_ + " -i " + addedQuetesFileName + " -f mp3 -vn " + addedQuetesSaveFileName;

        savingProcess_->setProcessChannelMode(QProcess::MergedChannels);
        savingProcess_->start(cmd_);
    }
}

void MainWindow::on_setFFmpegExe_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose executable"),
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0, QDir::homePath()),
        tr("Executable (*.exe);;All files (*.*)"));

    if (!fileName.isEmpty()) {
        ui->exeLabel->setText(fileName);
        ffmpegPath_ = "\"" + fileName + "\"";
    }
}
