#ifndef DB2_PG_MEMPOOL_CLIENT_H
#define DB2_PG_MEMPOOL_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <infiniband/verbs.h>
#include "c.h"
#include "access/logindex_hashmap.h"
#include "storage/bufpage.h"

struct RDMAReadPageInfo{
	struct ibv_mr remote_pa_mr, remote_pida_mr;
	size_t pa_ofs;
};
typedef struct RDMAReadPageInfo RDMAReadPageInfo;

extern bool PageExistsInMemPool(KeyType PageID, RDMAReadPageInfo* rdma_read_info);

extern bool FetchPageFromMemoryPool(char* des, KeyType PageID, RDMAReadPageInfo* rdma_read_info);

extern bool LsnIsSatisfied(PageXLogRecPtr PageLSN);

extern void ReplayXLog();

extern void AsyncAccessPageOnMemoryPool(KeyType PageID);

extern void AsyncGetNewestPageAddressTable();

extern void AsyncFlushPageToMemoryPool(char* src, KeyType PageID);


#ifdef __cplusplus
}
#endif

#endif //DB2_PG_MEMPOOL_CLIENT_H