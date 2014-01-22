/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qtexttospeech_p.h"
#include <jni.h>

#include <QtCore/private/qjni_p.h>
#include <QtCore/private/qjnihelpers_p.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QString dummyModule = QStringLiteral("dummy");

static jclass g_qtSpeechClass = 0;

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void */*reserved*/)
{
    typedef union {
        JNIEnv *nativeEnvironment;
        void *venv;
    } UnionJNIEnvToVoid;

    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
        return JNI_ERR;

    JNIEnv *jniEnv = uenv.nativeEnvironment;
    jclass clazz = jniEnv->FindClass("org/qtproject/qt5/android/speech/QtTextToSpeech");

    if (clazz) {
        g_qtSpeechClass = static_cast<jclass>(jniEnv->NewGlobalRef(clazz));
//        if (env->RegisterNatives(g_qtSpeechClass,
//                                 methods,
//                                 sizeof(methods) / sizeof(methods[0])) < 0) {
//            return false;
//        }

        qDebug() << "FOUND JCLASS QtSpeech";

        //QJNIObjectPrivate obj(res);
        //jniEnv->DeleteLocalRef(res);

//        jniEnv->call
//        g_qtSpeechClass
    }

    return JNI_VERSION_1_4;
}

//class QTextToSpeechVoicePrivateAndroid : public QTextToSpeechVoicePrivate
//{
//public:
//    QTextToSpeechVoicePrivateAndroid();
//    QString name() const;
//    QLocale locale() const;
//};


class QTextToSpeechPrivateAndroid : public QTextToSpeechPrivate
{
public:
    QTextToSpeechPrivateAndroid(QTextToSpeech *speech);
    ~QTextToSpeechPrivateAndroid();

    void say(const QString &text);
    void stop();
    void pause();
    void resume();

    void setRate(double rate);
    void setPitch(double pitch);
    void setVolume(double volume);
    QTextToSpeech::State state() const;

private:
    QJNIObjectPrivate m_speech;
};


QTextToSpeechPrivateAndroid::QTextToSpeechPrivateAndroid(QTextToSpeech *speech)
    : QTextToSpeechPrivate(speech)
{
    Q_ASSERT(g_qtSpeechClass);

    jobject activity = QtAndroidPrivate::activity();
    m_speech = QJNIObjectPrivate::callStaticObjectMethod(g_qtSpeechClass,
                                                           "open",
                                                           "(Landroid/content/Context;)Lorg/qtproject/qt5/android/speech/QtTextToSpeech;",
                                                           activity);
}

QTextToSpeechPrivateAndroid::~QTextToSpeechPrivateAndroid()
{
}

QTextToSpeech::QTextToSpeech(QObject *parent)
    : QObject(*new QTextToSpeechPrivateAndroid(this), parent)
{
    qRegisterMetaType<QTextToSpeech::State>();
}
//QTextToSpeechVoice::QTextToSpeechVoice()
//    : d(new QTextToSpeechVoicePrivateAndroid)
//{}

//QString QTextToSpeechVoicePrivateAndroid::name() const {
//    return QStringLiteral("default");
//}
//QLocale QTextToSpeechVoicePrivateAndroid::locale() const
//{
//    return QLocale();
//}

//QTextToSpeechVoicePrivateAndroid::QTextToSpeechVoicePrivateAndroid()
//    : QTextToSpeechVoicePrivate()
//{
//}

//QTextToSpeechVoice QTextToSpeechPrivate::currentVoice() const
//{
//    return QTextToSpeechVoice();
//}

//void QTextToSpeechPrivate::setVoice(const QTextToSpeechVoice &voice)
//{
////    m_currentVoice = voice;
//}

//QVector<QTextToSpeechVoice> QTextToSpeechPrivate::availableVoices() const
//{
//    QVector<QTextToSpeechVoice> voiceList;
//    return voiceList;
//}

//QVector<QString> QTextToSpeechPrivate::availableVoiceTypes() const
//{
//    QVector<QString> voiceTypes;
//    return voiceTypes;
//}

//void QTextToSpeechPrivate::setVoiceType(const QString& type)
//{
//}

//QString QTextToSpeechPrivate::currentVoiceType() const
//{
//    return QString();
//}

void QTextToSpeechPrivateAndroid::say(const QString &text)
{
    QJNIEnvironmentPrivate env;
    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(text.constData()),
                                        text.length());
    m_speech.callMethod<void>("say", "(Ljava/lang/String;)V", jstr);
}

QTextToSpeech::State QTextToSpeechPrivateAndroid::state() const
{
    return m_state;
}

void QTextToSpeechPrivateAndroid::stop()
{
//    QJNIEnvironmentPrivate env;
    m_speech.callMethod<void>("stop", "()V");
}

void QTextToSpeechPrivateAndroid::pause()
{
}

void QTextToSpeechPrivateAndroid::resume()
{
}

void QTextToSpeechPrivateAndroid::setPitch(double /*pitch*/)
{
}

void QTextToSpeechPrivateAndroid::setRate(double /*rate*/)
{
}

void QTextToSpeechPrivateAndroid::setVolume(double /*volume*/)
{
}

QT_END_NAMESPACE