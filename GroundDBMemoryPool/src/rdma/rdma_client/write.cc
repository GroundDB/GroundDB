#include <rdma.hh>
#include "../util.h"

namespace mempool{

int rdma_write(
    struct resources *res, /* RDMA Connection resources */
    struct memory_region *memreg,
    struct connection *conn,
    const char *buffer,    /* Local buffer to write from */
    const size_t size      /* number of bytes to write */
)
{
    // Attempt to perform rdma write
    memcpy(memreg->buf, buffer, size);
    if (post_send(res, memreg, conn, IBV_WR_RDMA_WRITE))
    {
        fprintf(stderr, "failed to post SR\n");
        return 1;
    }

    if (poll_completion(res, conn))
    {
        fprintf(stderr, "poll completion failed\n");
        return 1;
    }
    return 0;
}

} // namespace mempool