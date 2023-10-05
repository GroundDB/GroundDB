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
            tcp_port,
            ib_devname,
            ib_port))
    {
        fprintf(stderr, "failed to create resources\n");
        exit(1);
    }
    if (connect_qp(res, NULL, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        exit(1);
    }
    if (post_send(res, &res->memregs[0], &res->memregs[0].conns[0], IBV_WR_SEND))
    {
        fprintf(stderr, "failed to post SR\n");
        exit(1);
    }
    return res;
}

} // namespace mempool
