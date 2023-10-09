#include <rdma.hh>
#include "../util.h"

namespace mempool
{

    int rdma_write(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        char *buffer,     /* Local buffer to write from. nullptr if it is registered in memreg */
        const size_t lofs,  /* offset in local memory region */
        const size_t size, /* number of bytes to write. -1 denotes entire memreg */
        const size_t rofs  /* offset in remote memory region */
    )
    {
        // Attempt to perform rdma write
        if (buffer != nullptr)
            memcpy(memreg->buf + lofs, buffer, size);
        if (post_send(res, memreg, conn, IBV_WR_RDMA_WRITE, lofs, size, rofs))
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