#include "storage/GroundDB/rdma.hh"
#include "storage/GroundDB/util.h"

namespace mempool
{

// Same definition as in backend/access/logindex/logindex_hashmap.cpp
uint32_t HashKey(KeyType key) {
    uint32_t res = 0;
    res += (key.SpcID&0xFF) * 13 + 7;
    res += (key.DbID&0xFF) * 17 + 5;
    res += (key.RelID&0xFF) * 11 + 7;
    res += (key.ForkNum&0xFF) * 3 + 29;
    if(key.BlkNum != -1) {
        res += (key.BlkNum&0xFF) * 7 + 11;
    }
    return res;
}

uint32_t KeyTypeHashFunction::operator() (const KeyType &key) const {
    return HashKey(key);
}
bool KeyTypeEqualFunction::operator() (const KeyType &key1, const KeyType &key2) const {
    return key1.SpcID == key2.SpcID
        && key1.DbID == key2.DbID
        && key1.RelID == key2.RelID
        && key1.ForkNum == key2.ForkNum
        && key1.BlkNum == key2.BlkNum;
}

void PageAddressTable::append_page_array(size_t pa_size, const ibv_mr& pa_mr, const ibv_mr& pida_mr){
	std::unique_lock<std::shared_mutex> lk(mtx);
	idx_to_pid.emplace_back();
	idx_to_pid.back().resize(pa_size);
	for(int i = 0; i < pa_size; i++)
		idx_to_pid.back()[i] = nullKeyType;
	idx_to_mr.push_back(std::make_pair(pa_mr, pida_mr));
}
void PageAddressTable::at(KeyType pid, RDMAReadPageInfo& info){
	std::shared_lock<std::shared_mutex> lk(mtx);
	if(pid_to_idx.count(pid)){
		auto idx = pid_to_idx[pid];
		info.remote_pa_mr = idx_to_mr[idx.first].first;
		info.remote_pida_mr = idx_to_mr[idx.first].second;
		info.pa_ofs = idx.second;
	}
	else
		info.pa_ofs = -1;
}
void PageAddressTable::update(size_t pa_idx, size_t pa_ofs, KeyType pid){
	std::unique_lock<std::shared_mutex> lk(mtx);
	if(!KeyTypeEqualFunction()(idx_to_pid[pa_idx][pa_ofs], nullKeyType))
		pid_to_idx.erase(idx_to_pid[pa_idx][pa_ofs]);
	idx_to_pid[pa_idx][pa_ofs] = pid;
	pid_to_idx[pid] = std::make_pair(pa_idx, pa_ofs);
}

} // namespace mempool