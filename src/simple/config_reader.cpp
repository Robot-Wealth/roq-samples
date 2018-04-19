/* Copyright (c) 2017-2018, Hans Erik Thrane */

#include "simple/config_reader.h"

#include <ucl++.h>

#include <roq/logging.h>

#include <fstream>
#include <limits>
#include <map>

#include "common/config_variables.h"

namespace examples {
namespace simple {

namespace {
const auto NaN = std::numeric_limits<double>::quiet_NaN();
// Read and parse config file.
// Optionally expand variables using another config file.
static ucl::Ucl read_config_file(const std::string& config_file,
    const std::string& config_variables) {
  auto variables = common::ConfigVariables::read(config_variables);
  LOG(INFO) << "Reading config file \"" << config_file << "\"";
  std::ifstream file(config_file);
  LOG_IF(FATAL, file.fail()) << "Unable to read \"" << config_file << "\"";
  LOG(INFO) << "Parsing config file";
  std::string data((std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());
  std::string error;
  auto result = ucl::Ucl::parse(data, variables, error);
  LOG_IF(FATAL, !error.empty()) << error;
  return result;
}
// Create accounts
static std::map<std::string, std::pair<double, double> > create_accounts(
    const ucl::Ucl& setting) {
  std::map<std::string, std::pair<double, double> > result;
  for (auto& iter : setting) {
    auto key = iter.key();
    if (key.empty()) {
      // list format -- there are no positions
      result.emplace(iter.string_value(), std::make_pair(NaN, NaN));
    } else {
      // TODO(thraneh): also read start positions (but what format?)
      result.emplace(key, std::make_pair(NaN, NaN));
    }
  }
  return result;
}
// Create base config.
static common::Config create_base_config(const ucl::Ucl& setting) {
  common::Config result;
  for (auto& iter : setting) {
    LOG_IF(FATAL, iter.key() != "instrument") << "Expected instrument";
    common::Config::Instrument instrument {
      .exchange = iter.lookup("exchange").string_value(),
      .symbol   = iter.lookup("symbol").string_value(),
      .accounts = create_accounts(iter.lookup("accounts")),
      .risk_limit = iter.lookup("risk_limit").number_value(),
      .tick_size = iter.lookup("tick_size").number_value()
    };
    result.instruments.emplace_back(std::move(instrument));
  }
  return result;
}
// Create config object from parsed config file.
static Config create_config(const ucl::Ucl& setting) {
  Config result {
    .config    = create_base_config(setting.lookup("portfolio")),
    .weighted  = setting.lookup("weighted").bool_value(),
    .threshold = setting.lookup("threshold").number_value(),
    .quantity  = static_cast<double>(setting.lookup("quantity").int_value()),
  };
  LOG(INFO) << result;
  return result;
}
}  // namespace

Config ConfigReader::read(const std::string& config_file,
    const std::string& config_variables) {
  try {
    auto config = read_config_file(config_file, config_variables);
    auto root = config["strategy"];
    LOG_IF(FATAL, root.type() == UCL_NULL) <<
      "Root (strategy) was not found in the config file";
    return create_config(root);
  } catch (std::exception& e) {
    LOG(FATAL) << e.what();
    std::abort();  // FIXME(thraneh): avoid warning until LOG(FATAL) has been fixed
  }
}

}  // namespace simple
}  // namespace examples
