#include "storage/GroundDB/rdma.hh"
#include "storage/GroundDB/util.h"

namespace mempool
{

    int rdma_fetch(
        const struct resources *res, /* RDMA Connection resources */
        struct memory_region *memreg,
        const struct connection *conn,
        const size_t lofs,  /* offset in local memory region */
        const size_t size, /* number of bytes to read. -1 if entire menreg is to read */
        const size_t rofs  /* offset in remote memory region */
    )
    {
        auto send_wr_id = obtain_wr_id(memreg);
        if (post_send(res, memreg, conn, IBV_WR_RDMA_READ, send_wr_id, lofs, size, rofs))
        {
            fprintf(stderr, "failed to post SR\n");
            return 1;
        }

        if (poll_completion(res, memreg, send_wr_id))
        {
            fprintf(stderr, "poll completion failed\n");
            return 1;
        }
        return 0;
    }

} // namespace mempool