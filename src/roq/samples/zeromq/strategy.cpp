/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/samples/zeromq/strategy.hpp"

#include <fmt/format.h>

#include "roq/samples/zeromq/flags/flags.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace samples {
namespace zeromq {

namespace {
auto create_socket(auto &context) {
  zmq::Socket result(context, ZMQ_PUB);
  result.bind(flags::Flags::endpoint());
  return result;
}
}  // namespace

// initialise a strategy with a dispatcher and a socket (created from a context)
Strategy::Strategy(client::Dispatcher &dispatcher) : dispatcher_(dispatcher), socket_(create_socket(context_)) {
}

// implement the TopOfBook Event type operator
void Strategy::operator()(const Event<TopOfBook> &event) {
  auto &[message_info, top_of_book] = event;  // deconstruct message into its two parts (message_info and top_of_book)
  /*
   Example message:
    message_info={source=0, source_name="ftx", source_session_id="a2f73b39-dd97-45bf-842d-2be9a05b0c4a", source_seqno=31836, receive_time_utc=1651113981345409265ns, receive_time=482186573715285ns, source_send_time=482186573715285ns, source_receive_time=482186573136443ns, origin_create_time=482186573136443ns, origin_create_time_utc=1651113981344830599ns, is_last=true, opaque=0},
    top_of_book={stream_id=6, exchange="ftx", symbol="BTC/USD", layer={bid_price=39449, bid_quantity=0.5257000000000001, ask_price=39450, ask_quantity=8.0679}, update_type=INCREMENTAL, exchange_time_utc=1651113981333000000ns}
  */
  auto &layer = top_of_book.layer;  // note layer object in top_of_book message example above
  // json message
  // note! you can optimize this by pre-allocating a buffer and use fmt::format_to
  auto message = fmt::format(
      R"(["{}",{},{},{},{}])"sv,
      top_of_book.symbol,
      layer.bid_price,
      layer.bid_quantity,
      layer.ask_price,
      layer.ask_quantity);
  // note! you should use a higher verbosity level here to make it possible to avoid some logging
  log::info<0>("{}"sv, message);
  // send
  socket_.send(std::data(message), std::size(message), ZMQ_DONTWAIT);
}

}  // namespace zeromq
}  // namespace samples
}  // namespace roq

/* QUESTIONS
 * I don't understand Strategy initialisation in enough detail - syntax issue.
 * Where do I create the Dispatcher instance with which the Strategy is initialised? At the call site?
 * Need to check we're using appropriate ZMQ queues and socket types

*/
