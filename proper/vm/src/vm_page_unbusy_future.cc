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
  page_unbusy_job(workq_ptr);

  void run() noexcept override;
  static void start(cb_promise<page_ptr> out, shared_ptr<page_unbusy_job> self,
                    page_ptr in) noexcept;

 private:
  void start_(cb_promise<page_ptr> out, shared_ptr<page_unbusy_job> self,
              page_ptr in) noexcept;

  std::shared_ptr<void> self_;
  std::aligned_union_t<0, cb_promise<page_ptr>> out_;
  page_ptr pg_ = nullptr;
};


page_unbusy_job::page_unbusy_job(workq_ptr wq)
: workq_job(wq, TYPE_PERSIST | TYPE_PARALLEL)
{}

auto page_unbusy_job::run() noexcept -> void {
  /* Validate state. */
  assert(self_ != nullptr);
  assert(pg_ != nullptr);

  /* Check if page is busy. */
  if (pg_->get_flags() & page::fl_busy) return;

  /* Mark as ready. */
  void* voidprom = &out_;
  cb_promise<page_ptr>& prom = *static_cast<cb_promise<page_ptr>*>(voidprom);

  /* Cleanup. */
  deactivate();
  prom.set_value(std::move(pg_));
  prom.~cb_promise<page_ptr>();
  self_.reset();
}

auto page_unbusy_job::start(cb_promise<page_ptr> out,
                            shared_ptr<page_unbusy_job> self,
                            page_ptr in) noexcept -> void {
  assert(self != nullptr);
  assert(self->self_ == nullptr);

  self->start(std::move(out), self, std::move(in));
}

auto page_unbusy_job::start_(cb_promise<page_ptr> out,
                             shared_ptr<page_unbusy_job> self,
                             page_ptr in) noexcept -> void {
  /* Skip workq dance if page is not busy. */
  if (!(in->get_flags() & page::fl_busy)) {
    out.set_value(std::move(in));
    return;
  }

  /* Keep reference to self, in order to survive. */
  self_ = std::move(self);

  /* Initialize promise. */
  void* voidprom = &out_;
  new (voidprom) cb_promise<page_ptr>(std::move(out));

  /* Store page pointer. */
  pg_ = std::move(in);

  /* Start doing some work. */
  activate();
}


} /* namespace ilias::vm::<unnamed> */


auto page_unbusy_future(workq_service& wqs, cb_future<page_ptr>&& f) ->
    cb_future<page_ptr> {
  return page_unbusy_future(wqs.new_workq(), move(f));
}

auto page_unbusy_future(workq_ptr wq, cb_future<page_ptr>&& f) ->
    cb_future<page_ptr> {
  auto job = new_workq_job<page_unbusy_job>(std::move(wq));

  return async_lazy(pass_promise<page_ptr>(&page_unbusy_job::start),
                    std::move(job), std::move(f));
}

auto page_unbusy_future(workq_service& wqs, shared_cb_future<page_ptr> f) ->
    cb_future<page_ptr> {
  return page_unbusy_future(wqs.new_workq(), move(f));
}

auto page_unbusy_future(workq_ptr wq, shared_cb_future<page_ptr> f) ->
    cb_future<page_ptr> {
  auto job = new_workq_job<page_unbusy_job>(std::move(wq));

  return async_lazy(pass_promise<page_ptr>(&page_unbusy_job::start),
                    std::move(job), std::move(f));
}


}} /* namespace ilias::vm */
