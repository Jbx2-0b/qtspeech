TARGET = QtTextToSpeech
QT = core-private
DEFINES += QTEXTTOSPEECH_LIBRARY
MODULE = texttospeech
MODULE_PLUGIN_TYPES = texttospeech

load(qt_module)

HEADERS = \
    qtexttospeech.h \
    qtexttospeechplugin.h \
    qtexttospeechengine.h \
    qtexttospeech_p.h \
    qtexttospeech_global.h \
    qvoice.h \
    qvoice_p.h \

SOURCES = \
    qtexttospeech.cpp \
    qtexttospeechplugin.cpp \
    qtexttospeechengine.cpp \
    qvoice.cpp \
