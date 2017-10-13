include(../global.pri)

QT       += core multimedia
QT       -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

SOURCES += main.cpp \
    testCircularBuffer.cpp \
    testAngleDetector.cpp \
    testWavFile.cpp \
    testStereoRecordFilter.cpp \
    testAudioPipe.cpp

LIBS+= -lboost_unit_test_framework

WAV_DIR = "$$_PRO_FILE_PWD_/wav"
WAV_TESTS = "$$WAV_DIR/left.wav $$WAV_DIR/center.wav $$WAV_DIR/right.wav"

copyToDestdir($$WAV_TESTS, "wav")

uses(trikSound)
