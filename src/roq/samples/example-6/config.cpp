/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/samples/example-6/config.hpp"

#include "roq/samples/example-6/flags.hpp"

namespace roq {
namespace samples {
namespace example_6 {

void Config::dispatch(Handler &handler) const {
  // settings
  handler(client::Settings{
      .order_cancel_policy = OrderCancelPolicy::MANAGED_ORDERS,
      .order_management = {},
  });
  // accounts
  handler(client::Account{
      .regex = Flags::account(),
  });
  // symbols
  handler(client::Symbol{
      .regex = Flags::symbol_1(),
      .exchange = Flags::exchange_1(),
  });
  handler(client::Symbol{
      .regex = Flags::symbol_2(),
      .exchange = Flags::exchange_2(),
  });
  // currencies
  handler(client::Symbol{
      .regex = Flags::currencies(),
  });
}

}  // namespace example_6
}  // namespace samples
}  // namespace roq
