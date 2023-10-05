#include <rdma.hh>
#include "../util.h"

namespace mempool{

int rdma_write(
    struct resources *res, /* RDMA Connection resources */
    const char *buffer,    /* Local buffer to write from */
    const size_t size      /* number of bytes to write */
)
{
    // Attempt to perform rdma write
    memcpy(res->memregs[0].buf, buffer, size);
    if (post_send(res, &res->memregs[0], &res->memregs[0].conns[0], IBV_WR_RDMA_WRITE))
    {
        fprintf(stderr, "failed to post SR\n");
        return 1;
    }

    if (poll_completion(res, &res->memregs[0].conns[0]))
    {
        fprintf(stderr, "poll completion failed\n");
        return 1;
    }
    return 0;
}

} // namespace mempool