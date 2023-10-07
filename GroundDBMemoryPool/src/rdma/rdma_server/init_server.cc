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
    sleep(1);
    strcpy(memreg->buf, VERIFIER);
    fprintf(stdout, "going to send the message: '%s'\n", memreg->buf);
    if (post_send(res, memreg, conn, IBV_WR_SEND))
    {
        fprintf(stderr, "failed to post SR\n");
        exit(1);
    }
    // Remove until here
    return res;
}

} // namespace mempool
