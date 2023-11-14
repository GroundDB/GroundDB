#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "storage/GroundDB/pat.hh"
#include "storage/GroundDB/lru.hh"
#include "storage/GroundDB/request_buffer.h"
#include "storage/DSMEngine/ThreadPool.h"
#include "storage/DSMEngine/cache.h"

namespace mempool{

class MemPoolManager{
public:
    int tcp_port, ib_port;
    struct resources* res;
    struct page_array{
        struct memory_region* memreg;
        size_t size;
    };
    DSMEngine::ThreadPool *thrd_pool;
    DSMEngine::Cache *lru;
    std::vector<page_array> page_arrays;
    FreeList freelist;
    request_buffer *flush_page_reqbuf;
    request_buffer *access_page_reqbuf;
    request_buffer *sync_pat_reqbuf;
    std::vector<std::thread*> reqbuf_polling_threads;
    
    void init_resources(const int tcp_port, const char *ib_devname, const int ib_port);
    void init_thread_pool(size_t thrd_num);
    void allocate_page_array(size_t pa_size);

    void flush_page_handler(void* args);
    void init_flush_page_reqbuf();

    void access_page_handler(void* args);
    void init_access_page_reqbuf();

    void sync_pat_handler(void* args);
    void init_sync_pat_reqbuf();
};


} // namespace mempool