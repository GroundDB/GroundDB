#include <rdma.hh>
#include "../util.h"

namespace mempool{

int rdma_read(
    const struct resources *res, /* RDMA Connection resources */
    char *buffer,                /* Local buffer to read into */
    const size_t size            /* number of bytes to read */
)
{
    if (post_send(res, &res->memregs[0], &res->memregs[0].conns[0], IBV_WR_RDMA_READ))
    {
        fprintf(stderr, "failed to post SR\n");
        return 1;
    }

    if (poll_completion(res, &res->memregs[0].conns[0]))
    {
        fprintf(stderr, "poll completion failed\n");
        return 1;
    }
    memcpy(
        buffer,
        res->memregs[0].buf,
        size);
    return 0;
}

} // namespace mempool