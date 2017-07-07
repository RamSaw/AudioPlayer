#include "audioplayer.h"

#include <iostream>
#include <QMessageBox>
#include <Qt>

extern "C" {
#include <libavresample/avresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>

#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent), isSaveFinished_(true)
{
}

AudioPlayer::~AudioPlayer() {
    if (!isSaveFinished_) {
        QMessageBox msgBox;
        msgBox.setText("Previous saving is not finished");
        msgBox.exec();
    }

    if (saveThread_.joinable()) {
           saveThread_.join();
    }
}

void AudioPlayer::setFile(const QString &fileName) {
    fileName_ = fileName;
    player_.setMedia(QUrl::fromLocalFile(fileName_));
}

void AudioPlayer::play() {
    player_.play();
}

void AudioPlayer::stop() {
    player_.stop();
}

void AudioPlayer::save(const QString &saveFileName) {
    isSaveFinished_ = false;

    unsigned int audioIdStream;
    AVFormatContext* pInFormat = NULL;

    av_register_all();

    if (avformat_open_input(&pInFormat, fileName_.toStdString().c_str(), NULL, NULL) < 0)
        return;
    if (avformat_find_stream_info(pInFormat, NULL) < 0)
        return;

    /* Finding audio stream in video file */
    unsigned int i = 0;
    while (i < pInFormat->nb_streams) {
        if (pInFormat->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            break;
        }
        i++;
    }
    audioIdStream = i;
    if (audioIdStream >= pInFormat->nb_streams) {
        std::cerr << "No audio track in video file\n";
        qDebug() << "No audio track in video file";
        avformat_close_input(&pInFormat);
        return;
    }

    /* Finding decoder */
    AVCodecContext* pInCodecContext = pInFormat->streams[audioIdStream]->codec;
    AVCodec* codec = avcodec_find_decoder(pInCodecContext->codec_id);
    if (!codec) {
        std::cerr << "Could not find input codec\n";
        qDebug() << "Could not find input codec";
        avformat_close_input(&pInFormat);
        return;
    }
    int error = avcodec_open2(pInCodecContext, codec, NULL);
    if (error < 0) {
        std::cerr << "Could not open input codec\n";
        qDebug() << "Could not open input codec";
        avformat_close_input(&pInFormat);
        return;
    }

    /* Creating Output format */
    AVOutputFormat* fmt = av_guess_format("flac", NULL, NULL);
    if (!fmt) {
        std::cerr << "No flac format\n";
        qDebug() << "No flac format";
        avformat_close_input(&pInFormat);
        return;
    }

    AVFormatContext* pOutFormat = avformat_alloc_context();
    if (!pOutFormat) {
        std::cerr << "Memory allocation error\n";
        qDebug() << "Memory allocation error";
        avformat_close_input(&pInFormat);
        return;
    }
    pOutFormat->oformat = fmt;

    AVCodec* outCodec = avcodec_find_encoder(fmt->audio_codec);
    if (!outCodec)
        return;

    AVStream* outStream = avformat_new_stream(pOutFormat, outCodec);
    if (!outStream)
        return;

    outStream->codec->sample_fmt  = pInCodecContext->sample_fmt;
    outStream->codec->bit_rate    = pInCodecContext->bit_rate;
    outStream->codec->sample_rate = pInCodecContext->sample_rate;
    outStream->codec->channels    = pInCodecContext->channels;

    pOutFormat->streams[0]->codec = outStream->codec;
    pOutFormat->nb_streams = 1;

    error = avio_open(&pOutFormat->pb, saveFileName.toStdString().c_str(), AVIO_FLAG_WRITE);
    if (error < 0) {
        std::cerr << "Could not open file\n";
        qDebug() << "Could not open file";
        avformat_close_input(&pInFormat);
        avformat_close_input(&pOutFormat);
        return;
    }

    av_dump_format(pOutFormat, 0, saveFileName.toStdString().c_str(), 1);

    error = avformat_write_header(pOutFormat, NULL);

    AVPacket packet;
    av_init_packet(&packet);
    AVFrame* frame = av_frame_alloc();

    while (av_read_frame(pInFormat, &packet) >= 0) {
        int gotFrame, gotPacket;
        if (packet.stream_index == audioIdStream) {
            avcodec_decode_audio4(pInCodecContext, frame, &gotFrame, &packet);
            if (gotFrame) {
                avcodec_encode_audio2(outStream->codec, &packet, frame, &gotPacket);
                av_dup_packet(&packet);
                packet.stream_index = 0;
                //packet.pts = ; set if needed
                //packet.dts = ;
                error = av_write_frame(pOutFormat, &packet);
            }
        }
        av_free_packet(&packet);
    }

    avcodec_free_frame(&frame);
    avcodec_close(outStream->codec);
    avio_close(pOutFormat->pb);
    avformat_free_context(pOutFormat);
    avformat_close_input(&pInFormat);

    isSaveFinished_ = true;
}


void AudioPlayer::saveInOtherThread(const QString &saveFileName) {
    if (!isSaveFinished_) {
        QMessageBox msgBox;
        msgBox.setText("Previous saving is not finished");
        msgBox.exec();
    }
    if (saveThread_.joinable()) {
        saveThread_.join();
    }
    saveThread_ = std::thread(&AudioPlayer::save, this, saveFileName);
}
