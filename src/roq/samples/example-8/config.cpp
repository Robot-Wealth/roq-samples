/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/samples/example-8/config.hpp"

#include "roq/samples/example-8/flags.hpp"

namespace roq {
namespace samples {
namespace example_8 {

void Config::dispatch(Handler &handler) const {
  handler(client::Symbol{
      .regex = Flags::symbol(),
      .exchange = Flags::exchange(),
  });
}

}  // namespace example_8
}  // namespace samples
}  // namespace roq
