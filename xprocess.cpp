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
#include "xprocess.h"

XProcess::XProcess(QObject *parent) : QObject(parent)
{

}

QList<XProcess::PROCESS_INFO> XProcess::getProcessesList()
{
    QList<PROCESS_INFO> listResult;
#ifdef Q_OS_WIN
    PROCESSENTRY32W pe32;
    pe32.dwSize=sizeof(PROCESSENTRY32W);

    HANDLE hProcesses=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    if(hProcesses!=INVALID_HANDLE_VALUE)
    {
        if(Process32FirstW(hProcesses,&pe32))
        {
            do
            {
                PROCESS_INFO record;

                record.nID=pe32.th32ProcessID;
                record.nParentID=pe32.th32ParentProcessID;
                record.sName=QString::fromWCharArray(pe32.szExeFile);

                listResult.append(record);
            }
            while(Process32NextW(hProcesses,&pe32));
        }

        CloseHandle(hProcesses);
    }
#endif

    return listResult;
}
