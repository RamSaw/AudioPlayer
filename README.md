# AudioPlayer
An application to extract audio from video. You can play it with audio visualization by oscillogram and save it.

# Extended description
Application uses Qt5 as a base framework. For saving audio program runs ffmpeg executable with specific command. That is why if the program haven't found ffmpeg.exe in Windows you need to specify it. On linux it is supposed that ffmpeg command is available. Audio played by QMediaPlayer class and oscillogram painted by QCharts and QAudioProbe.
Links to download executables: https://ffmpeg.zeranoe.com/builds/ - Windows, Linux - you install from source or install from repositories.
