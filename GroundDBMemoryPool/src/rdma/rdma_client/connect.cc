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
    if (register_mr(res)){
        fprintf(stderr, "failed to register memory regions\n");
        return NULL;
    }
    if (connect_qp(res, &res->memregs[0], server_name, tcp_port, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        return NULL;
    }
    // Following lines are only for simulation and will be removed in the future.
    if (post_receive(res, &res->memregs[0], &res->memregs[0].conns[0]))
    {
        fprintf(stderr, "failed to post RR\n");
        return NULL;
    }
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
    return res;
}

} // namespace mempool