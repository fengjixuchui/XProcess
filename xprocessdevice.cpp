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
#include "xprocessdevice.h"

XProcessDevice::XProcessDevice(QObject *parent) :
    QIODevice(parent)
{
#ifdef Q_OS_WIN
    hProcess=nullptr;
    nPID=0;
    __nAddress=0;
    __nSize=0;
#endif
}

XProcessDevice::~XProcessDevice()
{
    if(isOpen())
    {
        close();
    }
}

qint64 XProcessDevice::size() const
{
    return __nSize;
}

bool XProcessDevice::isSequential() const
{
    return false;
}

bool XProcessDevice::seek(qint64 pos)
{
    if((pos<(qint64)__nSize)&&(pos>=0))
    {
        QIODevice::seek(pos);
        return true;
    }

    return false;
}

bool XProcessDevice::reset()
{
    return seek(0);
}

bool XProcessDevice::open(QIODevice::OpenMode mode)
{
    return false; // Use openPId or OpenHandle
}

bool XProcessDevice::atEnd() const
{
    return (bytesAvailable()==0);
}

void XProcessDevice::close()
{
    bool bSuccess=true;
#ifdef Q_OS_WIN

    if(nPID)
    {
        bSuccess=CloseHandle(hProcess);
    }

#endif

    if(bSuccess)
    {
        setOpenMode(NotOpen);
    }
}

qint64 XProcessDevice::pos() const
{
    return QIODevice::pos();
}

bool XProcessDevice::openPID(qint64 nPID, qint64 __nAddress, qint64 __nSize, QIODevice::OpenMode mode)
{
    bool bResult=true;

    setOpenMode(mode);

    this->nPID=nPID;
    this->__nAddress=__nAddress;
    this->__nSize=__nSize;

    quint32 nFlags=0;

#ifdef Q_OS_WIN

    if(mode==ReadOnly)
    {
        nFlags=PROCESS_VM_READ;
    }
    else if(mode==WriteOnly)
    {
        nFlags=PROCESS_VM_WRITE;
    }
    else if(mode==ReadWrite)
    {
        nFlags=PROCESS_ALL_ACCESS;
    }

    hProcess=OpenProcess(nFlags,0,(DWORD)nPID);

    bResult=(hProcess!=nullptr);
#endif

    return bResult;
}
#ifdef Q_OS_WIN
bool XProcessDevice::openHandle(HANDLE hProcess, qint64 __nAddress, qint64 __nSize, QIODevice::OpenMode mode)
{
    this->hProcess=hProcess;
    this->__nAddress=__nAddress;
    this->__nSize=__nSize;

    setOpenMode(mode);
    this->hProcess=hProcess;

    return true;
}
#endif
qint64 XProcessDevice::adjustSize(qint64 nSize)
{
    qint64 nPos=pos();
    qint64 _nSize=X_ALIGN_UP(nPos,0x1000)-nPos;

    if(_nSize==0)
    {
        _nSize=0x1000;
    }

    _nSize=qMin(_nSize,(qint64)(__nSize-nPos));
    qint64 nResult=qMin(nSize,_nSize);

    return nResult;
}
#ifdef Q_OS_WIN
void XProcessDevice::checkWindowsLastError()
{
    quint32 nLastErrorCode=GetLastError();

    if(nLastErrorCode)
    {
        LPSTR messageBuffer=nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     nullptr, nLastErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

        setErrorString(QString("%1: ").arg(nLastErrorCode,0,16)+QString::fromUtf8((char *)messageBuffer,size));

        //Free the buffer.
        LocalFree(messageBuffer);
    }
}
#endif
qint64 XProcessDevice::readData(char *data, qint64 maxSize)
{
    qint64 nResult=0;

    qint64 _nPos=pos();

    maxSize=qMin(maxSize,(qint64)(__nSize-_nPos));

    for(qint64 i=0; i<maxSize;)
    {
        qint64 nDelta=X_ALIGN_UP(_nPos,N_BUFFER_SIZE)-_nPos;

        if(nDelta==0)
        {
            nDelta=N_BUFFER_SIZE;
        }

        nDelta=qMin(nDelta,(qint64)(maxSize-i));
#ifdef Q_OS_WIN
        SIZE_T nSize=0;

        if(!ReadProcessMemory(hProcess,(LPVOID *)(_nPos+__nAddress),data,(SIZE_T)nDelta,&nSize))
        {
            break;
        }

        if(nSize!=nDelta)
        {
            break;
        }

#endif
#ifdef Q_OS_LINUX
        break;
#endif
        _nPos+=nDelta;
        data+=nDelta;
        nResult+=nDelta;
        i+=nDelta;
    }

#ifdef Q_OS_WIN
    checkWindowsLastError();
#endif
    return nResult;
}

qint64 XProcessDevice::writeData(const char *data, qint64 maxSize)
{
    qint64 nResult=0;

    qint64 _nPos=pos();

    maxSize=qMin(maxSize,(qint64)(__nSize-_nPos));

    for(qint64 i=0; i<maxSize;)
    {
        qint64 nDelta=X_ALIGN_UP(_nPos,N_BUFFER_SIZE)-_nPos;

        if(nDelta==0)
        {
            nDelta=N_BUFFER_SIZE;
        }

        nDelta=qMin(nDelta,(qint64)(maxSize-i));
#ifdef Q_OS_WIN
        SIZE_T nSize=0;

        if(!WriteProcessMemory(hProcess,(LPVOID *)(_nPos+__nAddress),data,(SIZE_T)nDelta,&nSize))
        {
            break;
        }

        if(nSize!=nDelta)
        {
            break;
        }

#endif
#ifdef Q_OS_LINUX
        break;
#endif
        _nPos+=nDelta;
        data+=nDelta;
        nResult+=nDelta;
        i+=nDelta;
    }

#ifdef Q_OS_WIN
    checkWindowsLastError();
#endif
    return nResult;
}

void XProcessDevice::setErrorString(const QString &str)
{
    QIODevice::setErrorString(str);
}
