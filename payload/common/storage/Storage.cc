#include <common/storage/Storage.hh>

#include <common/Arena.hh>
#include <payload/Lock.hh>

extern "C" {
#include <string.h>
}

void Storage::Init() {
    OSInitMessageQueue(&s_queue, s_messages.values(), s_messages.count());
    Array<u8, 4 * 1024> *stack = new (MEM2Arena::Instance(), 0x8) Array<u8, 4 * 1024>;
    OSThread *thread = new (MEM2Arena::Instance(), 0x4) OSThread;
    OSCreateThread(thread, Poll, nullptr, stack->values() + stack->count(), stack->count(), 11, 0);
    OSResumeThread(thread);
}

Storage::Storage() : m_next(nullptr) {}

void Storage::notify() {
    OSSendMessage(&s_queue, this, OS_MESSAGE_NOBLOCK);
}

void Storage::remove() {
    Lock<NoInterrupts> lock;
    Storage **next;
    for (next = &s_head; *next != this; next = &(*next)->m_next) {}
    *next = m_next;
}

void Storage::add() {
    Lock<NoInterrupts> lock;
    Storage **next;
    for (next = &s_head; *next && (*next)->priority() >= priority(); next = &(*next)->m_next) {}
    m_next = *next;
    *next = this;
}

Storage::StorageHandle::StorageHandle(const char *path) : m_storage(nullptr), m_prefix(nullptr) {
    Lock<NoInterrupts> lock;
    if (s_head && s_head->priority() > 0) {
        if (!strncmp(path, "main:", strlen("main:"))) {
            m_storage = s_head;
            m_prefix = "main:";
        }
    }
    if (!m_storage) {
        for (Storage *storage = s_head; storage; storage = storage->m_next) {
            if (!strncmp(path, storage->prefix(), strlen(storage->prefix()))) {
                m_storage = storage;
                m_prefix = storage->prefix();
                break;
            }
        }
    }
    if (m_storage) {
        m_storage->m_mutex.lock();
    }
}

Storage::StorageHandle::StorageHandle(const FileHandle &file)
    : m_storage(nullptr), m_prefix(nullptr) {
    Lock<NoInterrupts> lock;
    if (file.m_file) {
        m_storage = file.m_file->storage();
    }
    if (m_storage) {
        m_storage->m_mutex.lock();
    }
}

Storage::StorageHandle::StorageHandle(const DirHandle &dir)
    : m_storage(nullptr), m_prefix(nullptr) {
    Lock<NoInterrupts> lock;
    if (dir.m_dir) {
        m_storage = dir.m_dir->storage();
    }
    if (m_storage) {
        m_storage->m_mutex.lock();
    }
}

Storage::StorageHandle::~StorageHandle() {
    if (m_storage) {
        m_storage->m_mutex.unlock();
    }
}

void *Storage::Poll(void * /* param */) {
    while (true) {
        void *msg;
        OSReceiveMessage(&s_queue, &msg, OS_MESSAGE_BLOCK);
        Storage *storage = reinterpret_cast<Storage *>(msg);

        Lock<Mutex> lock(storage->m_mutex);
        storage->poll();
    }
}

OSMessageQueue Storage::s_queue;
Array<OSMessage, 1> Storage::s_messages;
