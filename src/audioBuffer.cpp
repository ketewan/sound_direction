#include "include/internal/audioBuffer.h"


AudioBuffer::AudioBuffer(const QByteArray& bytes, const QAudioFormat& format):
    mBytes(bytes)
  , mFormat(format)
{}

QAudioFormat AudioBuffer::audioFormat() const
{
    return mFormat;
}

qint64 AudioBuffer::size() const
{
    return mBytes.size();
}

qint64 AudioBuffer::sampleCount() const
{
    qint64 count = size() / bytesPerSample();
    if (mFormat.channelCount() == 2) {
        count /= 2;
    }
    return count;
}

void* AudioBuffer::data()
{
    return reinterpret_cast<void*>(mBytes.data());
}

const void*AudioBuffer::data() const
{
    return reinterpret_cast<const void*>(mBytes.data());
}

const void*AudioBuffer::constData() const
{
    return reinterpret_cast<const void*>(mBytes.constData());
}

AudioBuffer AudioBuffer::leftChannel() const
{
    QByteArray leftChl(size() / 2, '\0');
    for (int i = 0; i < sampleCount(); ++i) {
        const char* sample = mBytes.data() + i * bytesPerSample() * mFormat.channelCount();
        for (int j = 0; j < bytesPerSample(); ++j) {
            leftChl[i * bytesPerSample() + j] = sample[j];
        }
    }
    QAudioFormat fmt = mFormat;
    fmt.setChannelCount(1);
    return AudioBuffer(leftChl, fmt);
}

AudioBuffer AudioBuffer::rightChannel() const
{
    QByteArray rightChl(size() / 2, '\0');
    for (int i = 0; i < sampleCount(); ++i) {
        const char* sample = mBytes.data() + i * bytesPerSample() * mFormat.channelCount() + bytesPerSample();
        for (int j = 0; j < bytesPerSample(); ++j) {
            rightChl[i * bytesPerSample() + j] = sample[j];
        }
    }
    QAudioFormat fmt = mFormat;
    fmt.setChannelCount(1);
    return AudioBuffer(rightChl, fmt);
}

int AudioBuffer::bytesPerSample() const
{
    return mFormat.sampleSize() / 8;
}
