/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/algo/strategies/spread.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace algo {
namespace strategies {

// XXX
// positions
// OrderUpdate (to user)
// stale order update -=> fail (how?)

Spread::Spread(
    framework::Dispatcher &dispatcher,
    const framework::State &state,
    const std::string_view &routing_id,
    const CreateOrder &create_order)
    : Base(dispatcher, state, routing_id, create_order), side_(create_order.side), quantity_(create_order.quantity),
      price_(create_order.price) {
  // XXX tick size + min trade vol
  // XXX md ready
}

void Spread::operator()(const ModifyOrder &) {
  // user requests quantity or price change
}

void Spread::operator()(const CancelOrder &) {
  // user request cancelation
  // orders?
  // yes: remove + async ack
  // no: ack
}

void Spread::operator()(const Ready &) {
  log::debug("READY!"sv);
  for (auto &order_manager : order_managers_)
    order_manager.start();
  update();
}

void Spread::operator()(const NotReady &) {
  for (auto &order_manager : order_managers_)
    order_manager.stop();
}

void Spread::operator()(const Event<Timer> &) {
  // !ready -> try cancel orders?
}

void Spread::operator()(const Event<TopOfBook> &) {
}

void Spread::operator()(const Event<MarketByPriceUpdate> &) {
  // auto spread = current_spread();
  // log::info("spread={}"sv, spread);
  update();
  // XXX DEBUG
  // is_market_data_ready(0);
  // is_order_management_ready(0, account_);
  // supports(0, SupportType::MODIFY_ORDER);
}

void Spread::operator()(const Event<PositionUpdate> &) {
}

// note! it's quicker to loop all instead of looking up what instrument has updated
void Spread::update() {
  if (!ready())
    return;
  std::array<Layer, 2> best;
  if (extract(0, std::span{&best[0], 1}) == 0 || extract(1, std::span{&best[1], 1}) == 0)
    return;
  auto price = NaN;
  switch (side_) {
    case Side::UNDEFINED:
      assert(false);
      break;
    case Side::BUY:
      price = best[1].bid_price - price_;
      break;
    case Side::SELL:
      price = best[1].ask_price - price_;
      break;
  }
  if (!std::isnan(price)) {
    // log::debug("{} {} {}"sv, price, best[0], best[1]);
    order_managers_[0].set_target(quantity_, price);
  }
}

double Spread::current_spread() const {
  std::array<Layer, 2> best;
  if (extract(0, std::span{&best[0], 1}) == 0 || extract(1, std::span{&best[1], 1}) == 0)
    return NaN;
  // log::debug("{} {}"sv, best[0], best[1]);
  switch (side_) {
    case Side::UNDEFINED:
      assert(false);
      break;
    case Side::BUY:
      return best[0].ask_price - best[1].bid_price;
    case Side::SELL:
      return best[0].bid_price - best[1].ask_price;
  }
  return NaN;
}

}  // namespace strategies
}  // namespace algo
}  // namespace roq
