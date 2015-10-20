!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       += core gui network widgets

TARGET = Jamtaba2
TEMPLATE = app

HEADERS += \
    ../src/audio/core/PortAudioDriver.h \
    ../src/recorder/JamRecorder.h \
    ../src/recorder/ReaperProjectGenerator.h \
    ../src/ninjam/protocol/ServerMessageParser.h \
    ../src/ninjam/protocol/ServerMessages.h \
    ../src/ninjam/protocol/ClientMessages.h \
    ../src/loginserver/natmap.h \
    ../src/audio/codec.h \
    ../src/midi/portmididriver.h \
    ../src/audio/Resampler.h \
    ../src/audio/vorbis/VorbisDecoder.h \
    ../src/audio/vorbis/VorbisEncoder.h \
    ../src/persistence/Settings.h \
    ../src/audio/vst/VstPlugin.h \
    ../src/audio/vst/vsthost.h \
    ../src/geo/WebIpToLocationResolver.h \
    ../src/StandAloneMainController.h \
    ../src/gui/MainWindowStandalone.h

SOURCES += \
    ../src/main.cpp \
    ../src/audio/core/PortAudioDriver.cpp \
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    ../src/midi/portmididriver.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/persistence/Settings.cpp \
    ../src/audio/vst/VstPlugin.cpp \
    ../src/audio/vst/vsthost.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \
    ../src/StandAloneMainController.cpp \
    ../src/gui/MainWindowStandalone.cpp

INCLUDEPATH += $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \

DEPENDPATH +=  $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \

#please change this PATH
VST_SDK_PATH = "E:/Jamtaba2/VST3 SDK"

win32{

    win32-msvc*{#all msvc compilers
        !contains(QMAKE_TARGET.arch, x86_64) {
            message("msvc x86 build") ## Windows x86 (32bit) specific build here
            LIBS_PATH = "static/win32-msvc"
        } else {
            message("msvc x86_64 build") ## Windows x64 (64bit) specific build here
            LIBS_PATH = "static/win64-msvc"
        }


        CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH -lportaudio -lminimp3 -lportmidi -lvorbisfile -lvorbisenc -lvorbis -logg
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lportmidid -lvorbisfiled -lvorbisencd -lvorbisd -loggd
    }

    win32-g++{#MinGW compiler
        message("MinGW x86 build")
        LIBS_PATH = "static/win32-mingw"
        LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudio -lminimp3 -lportmidi -lvorbisfile -lvorbisenc -lvorbis -logg
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32

    RC_FILE = Jamtaba2.rc #windows icon
}


macx{
    message("Mac build")

    macx-clang-32 {
        message("i386 build") ## mac 32bit specific build here
        LIBS_PATH = "mac32"
    } else {
        message("x86_64 build") ## mac 64bit specific build here
        LIBS_PATH = "mac64"
    }

    LIBS += -framework CoreAudio
    LIBS += -framework CoreMidi
    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework CoreServices
    LIBS += -framework Carbon

    LIBS += -L$$PWD/libs/$$LIBS_PATH/ -lportaudio -lminimp3  -lportmidi -lvorbisfile -lvorbisenc -lvorbis -logg

    #mac osx doc icon
    ICON = Jamtaba.icns
}

INCLUDEPATH += $$VST_SDK_PATH/pluginterfaces/vst2.x/
