#pragma once

#include <common/Array.hh>
#include <common/storage/Storage.hh>
extern "C" {
#include <dolphin/OSThread.h>

#include <stdarg.h>
}

class LogFile {
public:
    static void Init();
    static void VPrintf(const char *format, va_list vlist);

private:
    static void *Run(void *param);
    static void RemoveOldLogFiles();
    static bool IsValidLogFile(const Storage::NodeInfo &nodeInfo);
    static bool ShouldRemoveLogFile(const Storage::NodeInfo &nodeInfo);

    static Array<u8, 8 * 1024> s_stack;
    static OSThread s_thread;
    static s64 s_startTime;
    static Array<Array<char, 0x4000>, 2> s_buffers;
    static u8 s_index;
    static u16 s_offset;
};
