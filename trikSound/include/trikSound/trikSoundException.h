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

#include <exception>

#include <QString>

#include "triksound_global.h"

namespace trikSound {

class TRIKSOUNDSHARED_EXPORT TrikSoundException: public std::exception
{
public:
    TrikSoundException(const char* msg):
        mMsg(msg)
    {}

    virtual ~TrikSoundException() throw()
    {}

    const char* what() const throw()
    {
        static QByteArray text = mMsg.toLatin1();
        return text.data();
    }

private:
    QString mMsg;
};

}
