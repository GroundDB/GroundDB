#include <thread>
#include "storage/GroundDB/mempool_server.h"
#include "storage/GroundDB/rdma.hh"
#include "storage/GroundDB/util.h"
#include "storage/GroundDB/rdma_server.hh"
#include "storage/DSMEngine/ThreadPool.h"
#include "storage/DSMEngine/cache.h"
#include "storage/GroundDB/request_buffer.h"

namespace mempool {

void MemPoolManager::init_resources(const int tcp_port,     /* server TCP port */
                 const char *ib_devname, /* server device name. If NULL, client will use the first
                                            found device */
                 const int ib_port       /* server IB port */
)
{
    this->tcp_port = tcp_port;
    this->ib_port = ib_port;
    res = new struct resources();
    if (!res)
    {
        fprintf(stderr, "failed to malloc struct resource\n");
        exit(1);
    }

    if (resources_create(
            res,
            NULL,
            ib_devname,
            ib_port))
    {
        fprintf(stderr, "failed to create resources\n");
        exit(1);
    }
    struct memory_region *memreg = nullptr;
    if (register_mr(memreg, res)){
        fprintf(stderr, "failed to register memory regions\n");
        exit(1);
    }
    struct connection *conn = nullptr;
    if (connect_qp(conn, res, memreg, NULL, tcp_port, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        exit(1);
    }
    // Following lines are only for simulation and will be removed in the future.
    strcpy(memreg->buf, VERIFIER);
    fprintf(stdout, "going to send the message: '%s'\n", memreg->buf);
    sock_sync_data(conn);
    if (post_send(res, memreg, conn, IBV_WR_SEND, obtain_wr_id(memreg)))
    {
        fprintf(stderr, "failed to post SR\n");
        exit(1);
    }
    // Remove until here
}

void MemPoolManager::init_thread_pool(size_t thrd_num){
    thrd_pool = new DSMEngine::ThreadPool();
    thrd_pool->SetBackgroundThreads(thrd_num);
}

void MemPoolManager::allocate_page_array(size_t pa_size){
    struct memory_region *pa = nullptr;
    allocate_page(pa, res, nullptr, pa_size);

    struct connection *conn = nullptr;
    if (connect_qp(conn, res, pa, NULL, tcp_port, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        exit(1);
    }
    
    freelist.init();
    lru = DSMEngine::NewLRUCache(pa_size, &freelist);

    page_arrays.push_back((struct page_array){.memreg = pa, .size = pa_size});
    for(size_t i = 0; i < pa_size; i++){
        auto pagemeta = new struct PageMeta;
        *pagemeta = (struct PageMeta){
            .page_addr = pa->buf + i * BLCKSZ,
            .page_id_addr = pa->buf + pa_size * BLCKSZ + i * sizeof(KeyType)
        };
        freelist.push_back(pagemeta);
    }
    // todo: multiple page_array
}

void MemPoolManager::flush_page_handler(void* args){
    auto req = (struct flush_page_request*)args;
    auto res = (struct flush_page_response*)(args + sizeof(flush_page_request));
    auto e = lru->LookupInsert(req->page_id, nullptr, 1,
        [](DSMEngine::Cache::Handle* handle){});
    auto pagemeta = (PageMeta*)e->value;
    std::unique_lock<std::shared_mutex> lk(e->rw_mtx);
    memcpy(pagemeta->page_addr, req->page_data, BLCKSZ);
    memcpy(pagemeta->page_id_addr, &req->page_id, sizeof(KeyType));
    lk.unlock();
    lru->Release(e);
    res->successful = true;
}
void MemPoolManager::init_flush_page_reqbuf(){
    flush_page_reqbuf = new request_buffer(res, flush_page_reqbuf->memreg_,
        sizeof(flush_page_request), sizeof(flush_page_response), 32
        );
    struct connection *conn;
    connect_qp(conn, res, flush_page_reqbuf->memreg_, NULL, tcp_port, -1, ib_port);
    flush_page_reqbuf->init_on_server_side();
    std::function<void(void *args)> func = [this](void* args){this->flush_page_handler(args);};
    auto polling_thrd = new std::thread(&request_buffer::poll_on_server_side,
        flush_page_reqbuf, thrd_pool, std::ref(func));
    reqbuf_polling_threads.emplace_back(polling_thrd);
}

void MemPoolManager::access_page_handler(void* args){
    auto req = (struct access_page_request*)args;
    auto res = (struct access_page_response*)(args + sizeof(access_page_request));
    auto e = lru->Lookup(req->page_id);
    lru->Release(e);
    res->successful = true;
}
void MemPoolManager::init_access_page_reqbuf(){
    access_page_reqbuf = new request_buffer(res, access_page_reqbuf->memreg_,
        sizeof(access_page_request), sizeof(access_page_response), 32
        );
    struct connection *conn;
    connect_qp(conn, res, access_page_reqbuf->memreg_, NULL, tcp_port, -1, ib_port);
    access_page_reqbuf->init_on_server_side();
    std::function<void(void *args)> func = [this](void* args){this->access_page_handler(args);};
    auto polling_thrd = new std::thread(&request_buffer::poll_on_server_side,
        access_page_reqbuf, thrd_pool, std::ref(func));
    reqbuf_polling_threads.emplace_back(polling_thrd);
}

void MemPoolManager::sync_pat_handler(void* args){
    auto req = (struct sync_pat_request*)args;
    auto res = (struct sync_pat_response*)(args + sizeof(sync_pat_request));
    auto&page_array = page_arrays[req->pa_idx];
    for(int i = 0; req->pa_ofs + i < page_array.size && i < 4096; i++)
        res->page_id_array[i] = *(KeyType*)(page_array.memreg->buf + BLCKSZ * page_array.size + (req->pa_ofs + i) * sizeof(KeyType));
}
void MemPoolManager::init_sync_pat_reqbuf(){
    sync_pat_reqbuf = new request_buffer(res, sync_pat_reqbuf->memreg_,
        sizeof(sync_pat_request), sizeof(sync_pat_response), 32
        );
    struct connection *conn;
    connect_qp(conn, res, sync_pat_reqbuf->memreg_, NULL, tcp_port, -1, ib_port);
    sync_pat_reqbuf->init_on_server_side();
    std::function<void(void *args)> func = [this](void* args){this->sync_pat_handler(args);};
    auto polling_thrd = new std::thread(&request_buffer::poll_on_server_side,
        sync_pat_reqbuf, thrd_pool, std::ref(func));
    reqbuf_polling_threads.emplace_back(polling_thrd);
}


} // namespace mempool
