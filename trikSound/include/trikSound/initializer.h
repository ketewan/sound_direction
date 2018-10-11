/* Copyright 2014 - 2016 Evgenii Moiseenko.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <memory>

#include "triksound_global.h"
#include "circularBufferQAdapter.h"
#include "singleChannelCircularBuffer.h"
#include "doubleChannelCircularBuffer.h"

#include "audioPipe.h"
#include "splitFilter.h"
#include "digitalAudioFilter.h"
#include "angleDetector.h"
#include "recordFilter.h"
#include "stereoRecordFilter.h"
#include "vadFilter.h"
#include "stereoVadFilter.h"
#include "vadFilterWrapper.h"

#include "audioDeviceManager.h"
#include "audioStream.h"
#include "fileAudioStream.h"
#include "captureAudioStream.h"
#include "wavFile.h"

#include "settings.h"

#include "audioDeviceManager.h"

#ifdef TRIK
    #include "trikAudioDeviceManager.h"
#endif

namespace trikSound {

template <typename Iter>
class TRIKSOUNDSHARED_EXPORT Initializer
{
public:

    typedef std::shared_ptr<QAudioFormat>               QAudioFormatPtr;
    typedef std::shared_ptr<CircularBufferQAdapter>     CircularBufferQAdapterPtr;
    typedef std::shared_ptr<AudioDeviceManager>         AudioDeviceManagerPtr;
    typedef std::shared_ptr<AudioStream>                AudioStreamPtr;

    typedef std::shared_ptr<StereoAudioPipe<Iter>>      AudioPipePtr;
    typedef std::shared_ptr<AngleDetector<Iter>>        AngleDetectorPtr;
    typedef std::shared_ptr<VadFilterWrapper<Iter>>     VadFilterWrapperPtr;

    // buffer capacity in terms of count of windows it stores
    static const int BUFFER_CAPACITY = 20;

    // exception during initialization
    class InitException : public TrikSoundException
    {
    public:
        InitException(const char* msg):
            TrikSoundException(msg)
        {}
    };

    Initializer(const Settings& settings);

    QAudioFormatPtr getQAudioFormat() const;
    CircularBufferQAdapterPtr getCircularBuffer() const;
    AudioDeviceManagerPtr getAudioDeviceManager() const;
    AudioStreamPtr getAudioStream() const;

    AudioPipePtr getAudioPipe() const;
    AngleDetectorPtr getAngleDetector() const;
    VadFilterWrapperPtr getVadWrapper() const;


private:

    typedef typename AudioFilter<Iter>::FilterPtr          FilterPtr;
    typedef typename StereoAudioFilter<Iter>::FilterPtr    StereoFilterPtr;

    void createQAudioFormat(const Settings& settings);
    void createCircularBuffer(const Settings& settings);
    void createAudioDeviceManager(const Settings& settings);
    void createAudioStream(const Settings& settings);

    void createAudioPipe(const Settings& settings);
    void createAngleDetector(const Settings& settings);
    void createVadWrapper(const Settings& settings);


    QAudioFormatPtr mAudioFormat;
    CircularBufferQAdapterPtr mCircularBuffer;
    AudioDeviceManagerPtr mDeviceManager;
    AudioStreamPtr mAudioStream;
    AudioPipePtr mAudioPipe;
    AngleDetectorPtr mAngleDetector;
    VadFilterWrapperPtr mVadWrapper;

};


template <typename Iter>
Initializer<Iter>::Initializer(const Settings& settings)
{
    createQAudioFormat(settings);
    createCircularBuffer(settings);
    createAudioDeviceManager(settings);
    createAudioStream(settings);
    createAngleDetector(settings);
    createAudioPipe(settings);
}

template <typename Iter>
typename Initializer<Iter>::QAudioFormatPtr Initializer<Iter>::getQAudioFormat() const
{
    return mAudioFormat;
}

template <typename Iter>
typename Initializer<Iter>::CircularBufferQAdapterPtr Initializer<Iter>::getCircularBuffer() const
{
    return mCircularBuffer;
}

template <typename Iter>
typename Initializer<Iter>::AudioDeviceManagerPtr Initializer<Iter>::getAudioDeviceManager() const
{
    return mDeviceManager;
}

template <typename Iter>
typename Initializer<Iter>::AudioStreamPtr Initializer<Iter>::getAudioStream() const
{
    return mAudioStream;
}

template <typename Iter>
typename Initializer<Iter>::AudioPipePtr Initializer<Iter>::getAudioPipe() const
{
    return mAudioPipe;
}

template <typename Iter>
typename Initializer<Iter>::AngleDetectorPtr Initializer<Iter>::getAngleDetector() const
{
    return mAngleDetector;
}

template <typename Iter>
typename Initializer<Iter>::VadFilterWrapperPtr Initializer<Iter>::getVadWrapper() const
{
    return mVadWrapper;
}

template <typename T>
std::shared_ptr<T> my_make_shared(std::string const & comment) {
    std::shared_ptr<T> r(new T(), [=](T * p) { std::cerr << comment;} );
    return r;
}

template <typename T, typename A>
std::shared_ptr<T> my_make_shared(A const &a, std::string const & comment) {
    std::shared_ptr<T> r(new T(a), [=](T * p) { std::cerr << comment;} );
    return r;
}

template <typename T, typename A, typename B>
std::shared_ptr<T> my_make_shared(A const &a, B const &b, std::string const & comment) {
    std::shared_ptr<T> r(new T(a, b), [=](T * p) { std::cerr << comment;} );
    return r;
}

template <typename T, typename A, typename B, typename C>
std::shared_ptr<T> my_make_shared(A const &a, B const &b, C const &c, std::string const & comment) {
    std::shared_ptr<T> r(new T(a, b, c), [=](T * p) { std::cerr << comment;} );
    return r;
}


template <typename T, typename A, typename B, typename C, typename D>
std::shared_ptr<T> my_make_shared(A const &a, B const &b, C const &c, D const &d, std::string const & comment) {
    std::shared_ptr<T> r(new T(a, b, c, d), [=](T * p) { std::cerr << comment;} );
    return r;
}

template <typename Iter>
void Initializer<Iter>::createQAudioFormat(const Settings& settings)
{
    if (!mAudioFormat) {
        mAudioFormat = my_make_shared<QAudioFormat>("deleted mAudioFormat");
        mAudioFormat->setSampleRate(settings.sampleRate());
        mAudioFormat->setSampleSize(settings.sampleSize());
        mAudioFormat->setSampleType(settings.sampleType());
        mAudioFormat->setCodec("audio/pcm");
        if (settings.singleChannelFlag()) {
            mAudioFormat->setChannelCount(1);
        }
        else {
            mAudioFormat->setChannelCount(2);
        }
        if (!mAudioFormat->isValid()) {
            throw InitException("Initialization error. Invalid audio format");
        }
    }
}

template <typename Iter>
void Initializer<Iter>::createCircularBuffer(const Settings& settings)
{
    if (!mCircularBuffer) {
        std::shared_ptr<CircularBuffer> buffer;
        if (settings.singleChannelFlag()) {
            buffer = my_make_shared<SingleChannelCircularBuffer>(BUFFER_CAPACITY * settings.windowSize(),
                                                                 "deleted single channel buffer");
        }
        else {
            buffer = my_make_shared<DoubleChannelCircularBuffer>(BUFFER_CAPACITY * settings.windowSize(),
                                                                 "deleted double channel buffer");
        }
        mCircularBuffer = my_make_shared<CircularBufferQAdapter>(buffer, "deleted mCircularBuffer");
        //mCircularBuffer = std::shared_ptr<CircularBufferQAdapter>(new CircularBufferQAdapter(buffer),
        //                                [](CircularBufferQAdapter* p){std::cout << "deleted";});
        mCircularBuffer->open(QIODevice::ReadWrite);
    }
}

template <typename Iter>
void Initializer<Iter>::createAudioDeviceManager(const Settings& settings)
{
    if (!mDeviceManager) {
//        createQAudioFormat(settings);
//        createCircularBuffer(settings);

        QAudioDeviceInfo dev = QAudioDeviceInfo::defaultInputDevice();
#ifdef TRIK
        mDeviceManager = my_make_shared<TrikAudioDeviceManager>(dev,
                                                                *mAudioFormat.get(),
                                                                mCircularBuffer,
                                                                settings.audioDeviceInitFlag(),
                                                                "deleted trik mDeviceManager");
#else
        mDeviceManager = my_make_shared<AudioDeviceManager>(dev, *mAudioFormat.get(), mCircularBuffer,
                                                            "deleted mDeviceManager");
#endif


    }
}

template <typename Iter>
void Initializer<Iter>::createAudioStream(const Settings& settings)
{
    if (!mAudioStream) {
//        createQAudioFormat(settings);
//        createAudioDeviceManager(settings);
//        createCircularBuffer(settings);

        QAudioFormat& fmt = *mAudioFormat.get();

        size_t windowSize = settings.windowSize();

        if (settings.fileInputFlag()) {
            mAudioStream = my_make_shared<FileAudioStream>(settings.inputWavFilename(),
                                                           windowSize * fmt.channelCount(),
                                                           "deleted mAudioStream");
        }
        else {
            mAudioStream = my_make_shared<CaptureAudioStream>(mDeviceManager,
                                                              mCircularBuffer,
                                                              windowSize * fmt.channelCount(),
                                                              "deleted mAudioStream");
        }
    }
}

template <typename Iter>
void Initializer<Iter>::createAudioPipe(const Settings& settings)
{
    if (!mAudioPipe) {
//        createQAudioFormat(settings);

        mAudioPipe = my_make_shared<StereoAudioPipe<Iter>>("deleted mAudioPipe");
        auto monoPipe = my_make_shared<AudioPipe<Iter>>("deleted monoPipe");

        if (settings.filteringFlag()) {
            FilterPtr filter = my_make_shared<DigitalAudioFilter<Iter>>("deleted filter");
            monoPipe->insertFilter(monoPipe->end(), filter);
        }

        if (settings.singleChannelFlag() && settings.vadFlag()) {
            createVadWrapper(settings);
            monoPipe->insertFilter(monoPipe->end(),
                                   std::static_pointer_cast<AudioFilter<Iter>>(mVadWrapper->getMonoVad()));
        }

        if (settings.singleChannelFlag() && settings.recordStreamFlag()) {
            auto wavFile = my_make_shared<WavFile>(settings.outputWavFilename(), "deleted wavFile");
            wavFile->open(WavFile::WriteOnly, *mAudioFormat.get());
            FilterPtr record = my_make_shared<RecordFilter<Iter>>(wavFile, "deleted record");
            monoPipe->insertFilter(monoPipe->end(), record);
        }

        if (!settings.singleChannelFlag() && settings.vadFlag()) {
            createVadWrapper(settings);
            mAudioPipe->insertFilter(mAudioPipe->end(),
                                     std::static_pointer_cast<StereoAudioFilter<Iter>>(mVadWrapper->getStereoVad()));
        }

        if (settings.angleDetectionFlag()) {
            //createAngleDetector(settings);
            mAudioPipe->insertFilter(mAudioPipe->end(),
                                     std::static_pointer_cast<StereoAudioFilter<Iter>>(mAngleDetector));
        }


        if (!settings.singleChannelFlag() && settings.recordStreamFlag()) {
            auto wavFile = my_make_shared<WavFile>(settings.outputWavFilename(), "deleted wavFile");
            wavFile->open(WavFile::WriteOnly, *mAudioFormat.get());
            StereoFilterPtr record = my_make_shared<StereoRecordFilter<Iter>>(wavFile, "deleted record");
            mAudioPipe->insertFilter(mAudioPipe->end(), record);
        }


        StereoFilterPtr split = my_make_shared<SplitFilter<Iter>>(monoPipe, "deleted split");
        mAudioPipe->insertFilter(mAudioPipe->begin(), split);
    }
}

template <typename Iter>
void Initializer<Iter>::createAngleDetector(const Settings& settings)
{
    if (!mAngleDetector) {
        if (!settings.angleDetectionFlag()) {
            return;
        }
        if (settings.singleChannelFlag()) {
            throw InitException("Initialization error."
                                "Angle detection enabled with single audio channel");
        }
        mAngleDetector = my_make_shared<AngleDetector<Iter>>(settings.sampleRate(),
                                                             settings.micrDist(),
                                                             settings.angleDetectionHistoryDepth(),
                                                             "deleted mAngleDetector");
    }
}

template <typename Iter>
void Initializer<Iter>::createVadWrapper(const Settings& settings)
{
    if (!mVadWrapper) {
        if (settings.singleChannelFlag()) {
            auto vad = my_make_shared<VadFilter<Iter>>(settings.vadThreshold(), "deleted vad");
            mVadWrapper = my_make_shared<VadFilterWrapper<Iter>>(vad, "deleted mVadWrapper");
        }
        else {
            auto vad = my_make_shared<StereoVadFilter<Iter>>(settings.vadThreshold(), "deleted vad");
            mVadWrapper = my_make_shared<VadFilterWrapper<Iter>>(vad, "deleted mVadWrapper");
        }
    }
}

}
