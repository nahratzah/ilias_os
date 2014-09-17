#include <ilias/vm/page_unbusy_future.h>
#include <ilias/vm/page.h>
#include <atomic>
#include <cassert>
#include <exception>
#include <memory>

namespace ilias {
namespace vm {
namespace {


class page_unbusy_job final
: public workq_job
{
 public:
  page_unbusy_job(workq_ptr wq, future<page_ptr> src)
  : workq_job(wq, TYPE_PERSIST | TYPE_PARALLEL),
    src_(src)
  {}

  page_unbusy_job() = delete;
  page_unbusy_job(const page_unbusy_job&) = delete;
  page_unbusy_job& operator=(const page_unbusy_job&) = delete;

  static future<page_ptr> install(workq_ptr wq, future<page_ptr> src) {
    using std::placeholders::_1;

    shared_ptr<page_unbusy_job> self = new_workq_job<page_unbusy_job>(move(wq),
                                                                      src);

    callback(src, bind(&src_callback, weak_ptr<page_unbusy_job>(self)),
             PROM_DEFER);
    return new_promise<page_ptr>(bind(&dst_callback, move(self), _1));
  }

 private:
  static bool activate_wqjob(const shared_ptr<page_unbusy_job>& self)
      noexcept {
    if (self == nullptr ||
        self->started_.exchange(true, memory_order_relaxed) != true)
      return false;

    page_unbusy_job& self_ref = *self;
    self_ref.self_ = self;
    self_ref.activate(ACT_IMMED);
    return true;
  }

  static void dst_callback(const shared_ptr<page_unbusy_job>& self,
                           promise<page_ptr> out) noexcept {
    using std::placeholders::_1;
    assert(self->src_.is_initialized());

    self->dst_ = move(out);
    if (!activate_wqjob(self))
      self->src_.start();
  }

  static void src_callback(const weak_ptr<page_unbusy_job>& weak_self)
      noexcept {
    shared_ptr<page_unbusy_job> self = weak_self.lock();
    if (self != nullptr) activate_wqjob(move(self));
  }

  void run() noexcept override {
    assert(src_.is_initialized() && dst_.is_initialized());
    assert(src_.ready());

    try {
      if (src_.get()->get_flags() & page::fl_busy) return;
      dst_.set(src_.move_or_copy());
    } catch (...) {
      dst_.set_exception(current_exception());
    }
    self_.reset();
    this->deactivate();
  }

  shared_ptr<page_unbusy_job> self_;
  future<page_ptr> src_;
  promise<page_ptr> dst_;
  atomic<bool> started_{ false };
};


} /* namespace ilias::vm::<unnamed> */


auto page_unbusy_future(workq_service& wqs, future<page_ptr> f) ->
    future<page_ptr> {
  return page_unbusy_future(wqs.new_workq(), move(f));
}

auto page_unbusy_future(workq_ptr wq, future<page_ptr> f) ->
    future<page_ptr> {
  if (f.ready() && !(f.get()->get_flags() & page::fl_busy)) return f;

  return page_unbusy_job::install(move(wq), move(f));
}


}} /* namespace ilias::vm */
