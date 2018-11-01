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

#include "doubleChannelCircularBuffer.h"

#include <algorithm>
#include <utility>

#include <QDebug>

#include "utils.h"

namespace trikSound
{

DoubleChannelCircularBuffer::DoubleChannelCircularBuffer(size_t capacity):
    mLeftBuffer(capacity / 2)
  , mRightBuffer(capacity / 2)
  , mLeftReadItr(mLeftBuffer.begin())
  , mRightReadItr(mRightBuffer.begin())
  , mLeftWriteItr(back_inserter(mLeftBuffer))
  , mRightWriteItr(back_inserter(mRightBuffer))
{}

quint64 DoubleChannelCircularBuffer::read(trikSound::sample_type* buf, size_t size)
{
    size_t halfSize = std::min(size / 2, samplesAvailable() / 2);
    auto leftReadEnd = mLeftReadItr + halfSize;
    auto rightReadEnd = mRightReadItr + halfSize;

    std::copy(mLeftReadItr, leftReadEnd, buf);
    std::copy(mRightReadItr, rightReadEnd, buf + halfSize);

    mLeftReadItr = leftReadEnd;
    mRightReadItr = rightReadEnd;
    if (leftReadEnd == mLeftBuffer.end()) {
        --mLeftReadItr;
        --mRightReadItr;
    }
    return halfSize * 2;
}

void DoubleChannelCircularBuffer::write(const sample_type* buf, size_t size)
{
    size_t halfSize = size / 2;

    bool overwriteFlag = false;
    int freeSpace = (mLeftReadItr - mLeftBuffer.begin()) + (mLeftBuffer.capacity() - mLeftBuffer.size());
    if (halfSize > freeSpace) {
        overwriteFlag = true;
    }

    // special case for writing to the empty container
    // in that case mReadItr == cb.begin() == cb.end()
    bool emptyFlag = mLeftBuffer.empty();

    extractChannel<2, 0>(buf, buf + size, mLeftWriteItr);
    extractChannel<2, 1>(buf, buf + size, mRightWriteItr);

    // redirect read iterator to begin in case of overwriting or empty buffer
    if (overwriteFlag || emptyFlag) {
        mLeftReadItr = mLeftBuffer.begin();
        mRightReadItr = mRightBuffer.begin();
    }
}

size_t DoubleChannelCircularBuffer::size() const
{
    return 2 * mLeftBuffer.size();
}

size_t DoubleChannelCircularBuffer::samplesAvailable() const
{
    return 2 * (mLeftBuffer.end() - mLeftReadItr);
}

void DoubleChannelCircularBuffer::resize(size_t size)
{
    mLeftBuffer.set_capacity(size / 2);
    mRightBuffer.set_capacity(size / 2);
    resetIterators();
}

void DoubleChannelCircularBuffer::clear()
{   
    mLeftBuffer.clear();
    mRightBuffer.clear();
    resetIterators();
}

int DoubleChannelCircularBuffer::channelCount() const
{
    return 2;
}

void DoubleChannelCircularBuffer::swap(DoubleChannelCircularBuffer& other)
{
    std::swap(mLeftBuffer, other.mLeftBuffer);
    std::swap(mRightBuffer, other.mRightBuffer);
    resetIterators();
    other.resetIterators();
}

void DoubleChannelCircularBuffer::resetIterators() {
    mLeftReadItr = mLeftBuffer.begin();
    mRightReadItr = mRightBuffer.begin();
    mLeftWriteItr = back_inserter(mLeftBuffer);
    mRightWriteItr = back_inserter(mRightBuffer);
}

}
