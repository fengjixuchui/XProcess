// copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef XPROCESS_H
#define XPROCESS_H

#include <QObject>
#ifdef Q_OS_WIN
#include <Windows.h>
#include <winternl.h>
#include <Tlhelp32.h>
#include <psapi.h>
#endif
#ifdef Q_OS_LINUX
#include <QDirIterator>
#endif

class XProcess : public QObject
{
    Q_OBJECT
public:
    struct PROCESS_INFO
    {
        QString sName;
//        qint64 nParentID;
        qint64 nID;
        QString sFilePath;
        qint64 nImageAddress;
        qint64 nImageSize;
    };

    explicit XProcess(QObject *parent = nullptr);
    static QList<PROCESS_INFO> getProcessesList();
#ifdef Q_OS_WIN
    static bool setPrivilege(char *pszName,bool bEnable);
#endif
#ifdef Q_OS_WIN
    static qint64 getPIDByHandle(HANDLE hProcess);
#endif
    static PROCESS_INFO getInfoByPID(qint64 nPID);
    static qint64 getImageSize(HANDLE hProcess,qint64 nImageBase);
    static QString getFileNameByHandle(HANDLE hHandle);
    static bool readData(HANDLE hProcess,qint64 nAddress,char *pBuffer,qint32 nBufferSize);
    static bool writeData(HANDLE hProcess,qint64 nAddress,char *pBuffer,qint32 nBufferSize);
signals:

public slots:
};

#endif // XPROCESS_H
