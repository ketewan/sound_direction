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

#include "controlFifo.h"

#include <algorithm>
#include <sstream>

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

#include "utils.h"

using namespace std;

ControlFifo::ControlFifo():
    mFile(stdin)
  , mNotifier(fileno(mFile), QSocketNotifier::Read)
{
    fill(mCmdBuffer, end(mCmdBuffer), 0);
    connect(&mNotifier, SIGNAL(activated(int)), this, SLOT(readData()));
    mNotifier.setEnabled(true);
}

void ControlFifo::processEvents()
{
    QCoreApplication::processEvents();
}

void ControlFifo::readData()
{
    struct NotificationGuard
    {
        NotificationGuard(QSocketNotifier* notifier):
            mNotifierPtr(notifier)
        {
            mNotifierPtr->setEnabled(false);
        }
        ~NotificationGuard()
        {
            mNotifierPtr->setEnabled(true);
        }

    private:
        QSocketNotifier* mNotifierPtr;
    };

    NotificationGuard guard(&mNotifier);
    if (fgets(mCmdBuffer, CMD_BUFFER_SIZE, mFile) != nullptr) {
        qDebug() << "before readComand";
        readCommand();
        qDebug() << "after readCommand";
        fill(mCmdBuffer, end(mCmdBuffer), 0);
    }
}

void ControlFifo::readCommand()
{
    qDebug() << "readCommand";
    QString qstr(mCmdBuffer);
    QTextStream in(&qstr, QIODevice::ReadOnly);

    QString cmd;
    in >> cmd;
    if (cmd == "set") {
        QString paramName;
        QString param;
        in >> paramName >> param;

        if (in.status() != QTextStream::Ok) {
            qDebug() << "ControlFifo error. Unrecognized command: " << qstr;
            return;
        }

        if (paramName == "angleHistoryDepth") {
            int historyDepth = convertParam<int>(paramName.toLatin1().data(), param,
                                                 "ControlFifo error. Cannot convert historyDepth to int."
                                                 "Command: " + qstr.toStdString());
            emit updateAngleDetectionHistoryDepth(historyDepth);
        }
        else if (paramName == "windowSize") {
            quint64 windowSize = convertParam<quint64>(paramName.toLatin1().data(), param,
                                          "ControlFifo error. Cannot convert windowSize to size_t."
                                          "Command: " + qstr.toStdString());
            emit updateWindowSize(windowSize);
        }
        else if (paramName == "volume") {
            double volume = convertParam<double>(paramName.toLatin1().data(), param,
                                                 "ControlFifo error. Cannot convert volume to double."
                                                 "Command: " + qstr.toStdString());
            emit updateVolume(volume);
        }
    }
}
