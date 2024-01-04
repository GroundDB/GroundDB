#include <mutex>
#include "storage/GroundDB/mempool_client.h"
#include "storage/GroundDB/rdma.hh"
#include "storage/DSMEngine/rdma_manager.h"

namespace mempool{

class MemPoolClient{
public:
    MemPoolClient();
    static MemPoolClient* Get_Instance();
    void AppendToPAT(size_t pa_idx);

    std::shared_ptr<DSMEngine::RDMA_Manager> rdma_mg;
    std::vector<std::thread> threads;
    PageAddressTable pat;
    DSMEngine::ThreadPool* thrd_pool;
};

MemPoolClient::MemPoolClient(){
    struct DSMEngine::config_t config = {
            NULL,  /* dev_name */
            122189, /* tcp_port */
            1,	 /* ib_port */
            1, /* gid_idx */
            0,
            0};
    rdma_mg = std::shared_ptr<DSMEngine::RDMA_Manager>(DSMEngine::RDMA_Manager::Get_Instance(&config));
    rdma_mg->Mempool_initialize(DSMEngine::PageArray, BLCKSZ, RECEIVE_OUTSTANDING_SIZE * BLCKSZ);
    rdma_mg->Mempool_initialize(DSMEngine::PageIDArray, sizeof(KeyType), RECEIVE_OUTSTANDING_SIZE * sizeof(KeyType));

	thrd_pool = new DSMEngine::ThreadPool();
    thrd_pool->SetBackgroundThreads(5);

    AppendToPAT(0);
}

void MemPoolClient::AppendToPAT(size_t pa_idx){
	ibv_mr recv_mr, send_mr;

	rdma_mg->Allocate_Local_RDMA_Slot(recv_mr, DSMEngine::Message);
	rdma_mg->post_receive<DSMEngine::RDMA_Reply>(&recv_mr, 1);
	rdma_mg->Allocate_Local_RDMA_Slot(send_mr, DSMEngine::Message);
	auto send_pointer = (DSMEngine::RDMA_Request*)send_mr.addr;
	auto req = &send_pointer->content.mr_info;
	send_pointer->command = DSMEngine::mr_info_;
	send_pointer->buffer = recv_mr.addr;
	send_pointer->rkey = recv_mr.rkey;
	req->pa_idx = pa_idx;
	rdma_mg->post_send<DSMEngine::RDMA_Request>(&send_mr, 1);

	ibv_wc wc[3] = {};
	std::string qp_type("main");
	rdma_mg->poll_completion(wc, 1, qp_type, true, 1);
	rdma_mg->poll_completion(wc, 1, qp_type, false, 1);

	auto res = &((DSMEngine::RDMA_Reply*)recv_mr.addr)->content.mr_info;
	pat.append_page_array(res->pa_mr.length / BLCKSZ, res->pa_mr, res->pida_mr);

	rdma_mg->Deallocate_Local_RDMA_Slot(send_mr.addr, DSMEngine::Message);
	rdma_mg->Deallocate_Local_RDMA_Slot(recv_mr.addr, DSMEngine::Message);
}

MemPoolClient* MemPoolClient::Get_Instance(){
    static MemPoolClient* client = nullptr;
    static std::mutex lock;
    lock.lock();
    if (client == nullptr)
        client = new MemPoolClient();
    lock.unlock();
    return client;
}

} // namespace mempool


bool PageExistsInMemPool(KeyType PageID, RDMAReadPageInfo* rdma_read_info) {
	auto client = mempool::MemPoolClient::Get_Instance();
	RDMAReadPageInfo info;
	client->pat.at(PageID, info);
	return info.pa_ofs != -1;
}

bool FetchPageFromMemoryPool(char* des, KeyType PageID, RDMAReadPageInfo* rdma_read_info){
	auto rdma_mg = mempool::MemPoolClient::Get_Instance()->rdma_mg;

	ibv_mr pa_mr, pida_mr;
	rdma_mg->Allocate_Local_RDMA_Slot(pa_mr, DSMEngine::PageArray);
	rdma_mg->Allocate_Local_RDMA_Slot(pida_mr, DSMEngine::PageIDArray);
	rdma_mg->RDMA_Read(&rdma_read_info->remote_pa_mr, &pa_mr, rdma_read_info->pa_ofs * sizeof(BLCKSZ), sizeof(BLCKSZ), IBV_SEND_SIGNALED, 1, 1, "main");
	rdma_mg->RDMA_Read(&rdma_read_info->remote_pida_mr, &pida_mr, rdma_read_info->pa_ofs * sizeof(KeyType), sizeof(KeyType), IBV_SEND_SIGNALED, 1, 1, "main");
	
	auto res_page = (uint8_t*)pa_mr.addr;
	auto res_id = (KeyType*)pida_mr.addr;
	if(!mempool::KeyTypeEqualFunction()(*res_id, PageID))
		return false;
	memcpy(des, res_page, BLCKSZ);

	rdma_mg->Deallocate_Local_RDMA_Slot(pa_mr.addr, DSMEngine::PageArray);
	rdma_mg->Deallocate_Local_RDMA_Slot(pida_mr.addr, DSMEngine::PageIDArray);
	return true;
}

bool LsnIsSatisfied(PageXLogRecPtr PageLSN){
	uint64_t pagelsn = PageXLogRecPtrGet(PageLSN);
	return true;
	// todo: consider secondary compute nodes
}

void ReplayXLog(){
}

void AsyncAccessPageOnMemoryPool(){
}

void AsyncGetNewestPageAddressTable(){
}

void AsyncFlushPageToMemoryPool(){
}