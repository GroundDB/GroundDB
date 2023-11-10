#include "storage/GroundDB/rdma.hh"
#include "storage/GroundDB/util.h"

namespace mempool
{

int allocate_page(struct memory_region *&memreg, struct resources *res, const char* src, size_t page_cnt, size_t page_size){
	return register_mr(memreg, res, src, page_cnt * (page_size + sizeof(KeyType)));
}

} // namespace mempool