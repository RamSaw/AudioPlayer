# AudioPlayer
An application to extract audio from video. You can play it with audio visualization by oscillogram and save it.

# Extended description
Application uses Qt5 as a base framework. Oscillogram and playing audio were realized using Qt5. To save an audio track you need to install ffmpeg library. If video has several audio tracks the program chooses the first one. The application can be compiled by qmake. Tested on Ubuntu 16.04 on *.avi, *.mkv, *.mp3, *.flac files with ffmpeg version 3.3.2.
