#include <rdma.hh>
#include "../util.h"

namespace mempool{

struct resources *connectServer(
    const char *server_name, /* server host name */
    const int tcp_port,      /* server TCP port */
    const char *ib_devname,  /* server device name. If NULL, client will use the first
                                found device */
    const int ib_port        /* server IB port */
)
{
    struct resources *res = new struct resources();
    if (!res)
    {
        fprintf(stderr, "failed to malloc struct resource\n");
        return NULL;
    }

    if (resources_create(
            res,
            server_name,
            ib_devname,
            ib_port))
    {
        fprintf(stderr, "failed to create resources\n");
        return NULL;
    }
    struct memory_region *memreg = nullptr;
    if (register_mr(memreg, res)){
        fprintf(stderr, "failed to register memory regions\n");
        return NULL;
    }
    struct connection *conn = nullptr;
    if (connect_qp(conn, res, memreg, server_name, tcp_port, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        return NULL;
    }
    // Following lines are only for simulation and will be removed in the future.
    if (post_receive(res, memreg, conn))
    {
        fprintf(stderr, "failed to post RR\n");
        return NULL;
    }
    sock_sync_data(conn);
    if (poll_completion(res, &res->memregs[0].conns[0]))
    {
        fprintf(stderr, "poll completion failed\n");
        return NULL;
    }
    if (strcmp(res->memregs[0].buf, VERIFIER))
    {
        fprintf(stderr, "failed to verify connection\n");
        return NULL;
    }
    fprintf(stdout, "Connection verified\n");
    // Remove until here

    auto pat_req_buf = new request_buffer(1<<20);
    init_pat_access_on_client(res, pat_req_buf, server_name, tcp_port, ib_port);
    return res;
}

void init_pat_access_on_client(
        struct resources* res,
        struct request_buffer* pat_req_buf,
        const char *server_name, /* server host name */
        const int tcp_port,     /* server TCP port */
        const int ib_port       /* server IB port */
){
    struct memory_region *pat_mr = nullptr;
    if (register_mr(pat_mr, res, (char*)pat_req_buf->content_, pat_req_buf->size_)){
        fprintf(stderr, "failed to register memory regions\n");
        exit(1);
    }
    struct connection *pat_conn = nullptr;
    if (connect_qp(pat_conn, res, pat_mr, server_name, tcp_port + 1, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        exit(1);
    }
    sock_sync_data(pat_conn);
}
std::pair<uintptr_t, uint64_t> get_page_address_from_mempool(
        const struct resources *res,
        const struct memory_region *mr,
        const struct connection *conn,
        KeyType page_id
){
    post_receive(res, mr, conn, 0, sizeof(uintptr_t) + sizeof(uint64_t));
    *(KeyType*)mr->buf = page_id;
    post_send(res, mr, conn, IBV_WR_SEND, 0, sizeof(KeyType));
    poll_completion(res, conn);
    poll_completion(res, conn);
    uintptr_t addr = *(uintptr_t*)mr->buf;
    uint64_t LSN = *(uint64_t*)(mr->buf + sizeof(uintptr_t));
    return std::make_pair(addr, LSN);
}

} // namespace mempool