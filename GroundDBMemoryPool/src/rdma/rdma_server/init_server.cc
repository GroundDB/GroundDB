#include <thread>
#include <rdma.hh>
#include "../util.h"

namespace mempool {

struct resources* init_server(const int tcp_port,     /* server TCP port */
                 const char *ib_devname, /* server device name. If NULL, client will use the first
                                            found device */
                 const int ib_port       /* server IB port */
)
{
    struct resources *res = new struct resources();
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
        return NULL;
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
    if (post_send(res, memreg, conn, IBV_WR_SEND))
    {
        fprintf(stderr, "failed to post SR\n");
        exit(1);
    }
    // Remove until here

    // initialize PAT
    auto pat = new page_address_table();
    auto pat_req_buf = new request_buffer(1<<20);
    init_pat_on_server(res, pat, pat_req_buf, tcp_port, ib_port);
    return res;
}

void init_pat_on_server(
        struct resources* res,
        struct page_address_table *pat,
        struct request_buffer* pat_req_buf,
        const int tcp_port,     /* server TCP port */
        const int ib_port       /* server IB port */
){
    struct memory_region *pat_mr = nullptr;
    if (register_mr(pat_mr, res, (char*)pat_req_buf->content_, pat_req_buf->size_)){
        fprintf(stderr, "failed to register memory regions\n");
        exit(1);
    }
    struct connection *pat_conn = nullptr;
    if (connect_qp(pat_conn, res, pat_mr, NULL, tcp_port, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        exit(1);
    }
    if (post_receive(res, pat_mr, pat_conn, 0, sizeof(KeyType)))
    {
        fprintf(stderr, "failed to post RR\n");
        exit(1);
    }
    sock_sync_data(pat_conn);
    auto service = new std::thread(get_page_address_service, res, pat_mr, pat_conn, pat);
    service->detach();
}

void get_page_address_service(
        struct resources* res,
        struct memory_region* mr,
        struct connection* conn,
        struct page_address_table *pat
){
    while(true){
        poll_completion(res, conn, 0);
        post_receive(res, mr, conn, 0, sizeof(KeyType));
        KeyType page_id = *(KeyType*)mr->buf;
        uintptr_t addr = pat->pat[page_id];
        *(uintptr_t*)mr->buf = addr;
        post_send(res, mr, conn, IBV_WR_SEND, 0, sizeof(uintptr_t));
        poll_completion(res, conn);
    }
}

} // namespace mempool
