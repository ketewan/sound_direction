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

#include "outputFifo.h"

#include <QDebug>

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "trikSound/types.h"

using namespace std;
using trikSound::threshold_type;

OutputFifo::OutputFifo(const ViewSettings& settings):
    mOut(stdout, QIODevice::WriteOnly)
  , mWriteToFifo(settings.writeToFifo())
  , mShowAngle(settings.showAngle())
  , mShowVad(settings.showVadCoef())
  , mDiffTime(settings.diffTime())

  , mEventFlag(false)
  , mPrevTimestamp(0)

  , mFrameCnt(0)
  , mEnrg(0)
{
    mOut.setRealNumberNotation(QTextStream::FixedNotation);
    mOut.setRealNumberPrecision(12);
    mOut.setFieldWidth(4);

    if (settings.writeToFifo()) {
        //mkfifo(settings.fifoName().toStdString().c_str(), 0644);
        //Open for read and write to use non blocking mode
        auto fd_r = open(settings.fifoName().toStdString().c_str(), O_NONBLOCK | O_RDONLY);
        auto fd_w = open(settings.fifoName().toStdString().c_str(), O_NONBLOCK | O_WRONLY);
        if (fd_r < 0 || fd_w < 0)
             printf("\n %s \n", strerror(errno));
        mFileDescriptor = std::make_pair(fd_w, fd_r);
    }

    mAngles.reserve(100);
}

void OutputFifo::recieve(const trikSound::AudioEvent& event)
{
    if (event.vadIsActiveSetFlag() && event.vadIsActive()) {
        saveEventData(event);
        if (!mEventFlag) {
            mEventFlag = true;
            mPrevTimestamp = event.timestamp();
        }
    }
    if ( (double) abs(event.timestamp() - mPrevTimestamp) / TO_MS_COEFF >= mDiffTime) {
        printEventData(event);
        mEventFlag = false;
    }
}

void OutputFifo::printEventData(const trikSound::AudioEvent& event)
{
    if (mShowAngle && !mAngles.empty()) {
        int med = mAngles.size() / 2;
        nth_element(mAngles.begin(), mAngles.begin() + med, mAngles.end());
        int angle = mAngles[med];

        if (!mWriteToFifo)
            mOut << angle << endl;
        else {
            std::string buf = std::to_string(angle);
            buf += "\n";
            if ((write(mFileDescriptor.first, buf.c_str(), buf.length())) < 0) {
                printf("\n %s \n", strerror(errno));
            }
        }
    }

    if (mShowVad) {
        if (event.vadIsActive()) {
            threshold_type thrsd = mEnrg / mFrameCnt;

            if (!mWriteToFifo) {
                mOut << thrsd << endl;
            }
            else {
                std::string buf = std::to_string(thrsd);
                buf += "\n";
                if ((write(mFileDescriptor.first, buf.c_str(), buf.length())) < 0) {
                    printf("\n %s \n", strerror(errno));
                }
            }
        }
    }

    mAngles.clear();
    mEnrg = 0;
    mFrameCnt = 0;
}

void OutputFifo::saveEventData(const trikSound::AudioEvent& event)
{
    mFrameCnt++;
    if (mShowAngle && event.angleSetFlag()) {
        int angle = round(event.angle());
        mAngles.push_back(angle);
    }
    if (mShowVad && event.vadCoefSetFlag()) {
         mEnrg += event.vadCoef();
    }
}
