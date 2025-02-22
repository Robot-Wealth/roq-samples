/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/algo/framework/instrument.hpp"

using namespace std::literals;

namespace roq {
namespace algo {
namespace framework {

namespace {
const Mask<SupportType> REQUIRED_MARKET_DATA{
    SupportType::REFERENCE_DATA,
    SupportType::MARKET_STATUS,
    SupportType::MARKET_BY_PRICE,
};

const Mask<SupportType> REQUIRED_ORDER_MANAGEMENT{
    SupportType::CREATE_ORDER,
    SupportType::CANCEL_ORDER,
};
}  // namespace

bool Instrument::ready(
    const cache::Gateway &gateway, const cache::Market &market, const std::string_view &account) const {
  if (!gateway.ready(REQUIRED_MARKET_DATA))
    return false;
  if (utils::compare(market.reference_data.tick_size, 0.0) == 0)
    return false;
  // XXX this might not be the right place...
  if (utils::compare(market.market_status.trading_status, TradingStatus::OPEN) != 0)
    return false;
  if (!gateway.ready(REQUIRED_ORDER_MANAGEMENT, account))
    return false;
  return true;
}

}  // namespace framework
}  // namespace algo
}  // namespace roq
