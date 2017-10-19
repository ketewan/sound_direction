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

#include "trikSound/types.h"

using namespace std;
using trikSound::threshold_type;


OutputFifo::OutputFifo(const ViewSettings& settings):
    mOut(stdout, QIODevice::WriteOnly)
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

    mAngles.reserve(100);
}

void OutputFifo::recieve(const trikSound::AudioEvent& event)
{
    if ( (double) abs(event.timestamp() - mPrevTimestamp) / TO_MS_COEFF >= mDiffTime) {
        printEventData(event);
        mEventFlag = false;
    }
    if (event.vadIsActiveSetFlag() && event.vadIsActive()) {
        saveEventData(event);
        if (!mEventFlag) {
            mEventFlag = true;
            mPrevTimestamp = event.timestamp();
        }
    }
    else {
//        mOut << "----" << endl;
    }
}

void OutputFifo::printEventData(const trikSound::AudioEvent& event)
{
    static QString delim = "  ";

    if (mShowAngle && !mAngles.empty()) {
        int med = mAngles.size() / 2;
        nth_element(mAngles.begin(), mAngles.begin() + med, mAngles.end());
        int angle = mAngles[med];

        mOut << angle << delim;
        mOut << endl;
    }

    if (mShowVad) {
        threshold_type thrsd = mEnrg / mFrameCnt;
        if (thrsd > 0) {
            mOut << thrsd << delim;
            mOut << endl;
        }
    }
    mOut.flush();
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
