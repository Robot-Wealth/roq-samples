/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/samples/algo-proto/config.hpp"

#include <absl/container/flat_hash_set.h>

#include <toml++/toml.h>

#include <utility>
#include <vector>

#include "roq/logging.hpp"

#include "roq/samples/algo-proto/flags/flags.hpp"

using namespace std::literals;

namespace roq {
namespace samples {
namespace algo_proto {

namespace {
template <typename Callback>
bool find(auto &node, const std::string_view &key, Callback callback) {
  if (!node.is_table()) {
    log::warn("Unexpected: node is not a table"sv);
    return false;
  }
  auto table = node.as_table();
  auto iter = table->find(key);
  if (iter == table->end())
    return false;
  callback((*iter).second);
  return true;
}

template <typename R>
auto parse_instrument(auto &node) {
  R result;
  auto table = node.as_table();
  for (auto &[key, value] : *table) {
    auto name = static_cast<std::string_view>(key);
    if (name.compare("exchange"sv) == 0) {
      result.exchange = *value.template value<std::string_view>();
    } else if (name.compare("symbol"sv) == 0) {
      result.symbol = *value.template value<std::string_view>();
    } else if (name.compare("weight"sv) == 0) {
      result.weight = *value.template value<double>();
    } else {
      log::fatal(R"(Unexpected: key="{}")"sv, name);
    }
  }
  if (std::empty(result.exchange) || std::empty(result.symbol))
    log::fatal("Unexpected: exchange and symbol required"sv);
  return result;
}

template <typename R>
auto parse_instruments(auto &node) {
  R instruments;
  using value_type = typename R::value_type;
  auto array = node.as_array();
  for (auto &item : *array) {
    auto instrument = parse_instrument<value_type>(item);
    instruments.emplace_back(std::move(instrument));
  }
  return instruments;
}

auto parse_strategy(auto &node) {
  algo::framework::Config strategy;
  auto table = node.as_table();
  for (auto &[key, value] : *table) {
    auto name = static_cast<std::string_view>(key);
    if (name.compare("type"sv) == 0) {
      strategy.type = *value.template value<std::string_view>();
    } else if (name.compare("instruments"sv) == 0) {
      strategy.instruments = parse_instruments<decltype(strategy.instruments)>(value);
    } else {
      log::fatal(R"(Unexpected: key="{}")"sv, name);
    }
  }
  if (std::empty(strategy.type) || std::empty(strategy.instruments))
    log::fatal("Unexpected: instruments is empty"sv);
  return strategy;
}
}  // namespace

Config::Config(const std::string_view &path) {
  auto root = toml::parse_file(path);
  if (find(root, "strategies"sv, [&](auto &node) {
        auto table = node.as_table();
        for (auto &[key, value] : *table)
          strategies.emplace(key, parse_strategy(value));
      })) {
  } else {
    log::fatal("Expected: strategies"sv);
  }
  // extract
  for (auto &[_, strategy] : strategies) {
    for (auto &instrument : strategy.instruments)
      exchange_symbols[static_cast<std::string_view>(instrument.exchange)].emplace(
          static_cast<std::string_view>(instrument.symbol));
  }
}

void Config::dispatch(Handler &handler) const {
  // settings
  handler(client::Settings{
      .order_cancel_policy = OrderCancelPolicy::BY_ACCOUNT,
      .order_management = {},
  });
  // accounts
  handler(client::Account{
      .regex = flags::Flags::account(),
  });
  // symbols
  for (auto &[exchange, symbols] : exchange_symbols)
    for (auto &symbol : symbols)
      handler(client::Symbol{
          .regex = symbol,
          .exchange = exchange,
      });
  // currencies
  // - don't care about funding
}

}  // namespace algo_proto
}  // namespace samples
}  // namespace roq
