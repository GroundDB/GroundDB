#pragma once
#include <functional>
#include <mutex>
#include "storage/GroundDB/util.h"
#include "storage/DSMEngine/ThreadPool.h"

namespace DSMEngine{
	struct RDMA_Request;
}

namespace mempool{

struct flush_page_request{
	uint8_t page_data[BLCKSZ];
	KeyType page_id;
};
struct flush_page_response{
	bool successful;
};

struct access_page_request{
	KeyType page_id;
};
struct access_page_response{
	bool successful;
};

struct sync_pat_request{
	size_t pa_idx;
	size_t pa_ofs;
};
// Now only support fixed size
struct sync_pat_response{
	KeyType page_id_array[4096];
};

struct request_handler_args{
	DSMEngine::RDMA_Request* request;
	std::string client_ip;
	uint16_t compute_node_id;
};

class request_buffer {
public:
    uint8_t *content_;
	std::map<uint64_t, size_t> wr_id_to_idx_;
    size_t request_size_;
	size_t response_size_;
	size_t capacity_;
	size_t tot_size_;
	struct resources *res_;
	struct memory_region *memreg_;

    request_buffer(struct resources *res, struct memory_region* memreg, size_t request_size, size_t response_size, size_t capacity):
		res_(res),
		memreg_(memreg),
		request_size_(request_size),
		response_size_(response_size),
		capacity_(capacity)
	{
		tot_size_ = (request_size_ + response_size_) * capacity_;
		content_ = new uint8_t[tot_size_]();
		register_mr(memreg_, res_, (char*)content_, tot_size_);
	}
    ~request_buffer() {delete[] content_;}

	void init_on_server_side(){
		assert(memreg_->conns.size() == 1); // for now, we only have one compute node.
		for(int i=0; i<capacity_; i++){
			uint64_t wr_id = obtain_wr_id(memreg_);
			wr_id_to_idx_[wr_id] = i;
			post_receive(res_, memreg_, &memreg_->conns[0],
				wr_id, i * (request_size_ + response_size_), request_size_);
		}
	}

	// Run func for the request in the idx-th slot
	void run_handler(std::function<void(void *args)> func, uint64_t idx){
		func((void*)(content_ + idx * (request_size_ + response_size_)));
		auto wr_id = obtain_wr_id(memreg_);
		post_send(res_, memreg_, &memreg_->conns[0], IBV_WR_SEND,
			wr_id, idx * (request_size_ + response_size_) + request_size_, response_size_);
		poll_completion(res_, memreg_, wr_id);
		wr_id = obtain_wr_id(memreg_);
		wr_id_to_idx_[wr_id] = idx;
		post_receive(res_, memreg_, &memreg_->conns[0],
			wr_id, idx * (request_size_ + response_size_), request_size_);
	}
	struct reqbuf_handler_args{
		request_buffer* req_buf;
		std::function<void(void *args)> func;
		uint64_t idx;
	};
	static void reqbuf_handler(void* args){
		auto Args = (reqbuf_handler_args*)args;
		Args->req_buf->run_handler(Args->func, Args->idx);
	}
	void poll_on_server_side(DSMEngine::ThreadPool* thread_pool, std::function<void(void *args)> &func){
		while(true){
			uint64_t wr_id;
			poll_any_completion(res_, memreg_, wr_id, request_size_);
			auto idx = wr_id_to_idx_[wr_id];
			wr_id_to_idx_.erase(wr_id);
			struct reqbuf_handler_args reqbuf_args = {
				.req_buf = this,
				.func = func,
				.idx = idx
			};
			thread_pool->Schedule(reqbuf_handler, (void*)&reqbuf_args, rand() % thread_pool->total_threads_limit_);
		}
	}
};


} // namespace mempool