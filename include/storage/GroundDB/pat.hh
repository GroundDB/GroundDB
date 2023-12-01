#pragma once
#include "c.h"
#include "access/logindex_hashmap.h"
#include "storage/bufpage.h"

// todo: OpenAurora/include/port.h define following macros which causes link error. We undef them for now.
#undef printf
#undef sprintf
#undef fprintf

namespace mempool{

int allocate_page(struct memory_region *&memreg, struct resources *res, const char* src, size_t page_cnt, size_t page_size = BLCKSZ);

} // namespace mempool