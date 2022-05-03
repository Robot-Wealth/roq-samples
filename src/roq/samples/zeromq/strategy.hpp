/* Copyright (c) 2017-2022, Hans Erik Thrane */

#pragma once

#include <arpa/inet.h>

#include "roq/api.hpp"
#include "roq/client.hpp"

// note! thin wrappers around libzmq
#include "roq/samples/zeromq/zmq/context.hpp"
#include "roq/samples/zeromq/zmq/socket.hpp"

namespace roq {
namespace samples {
namespace zeromq {

// strategy implementation

// final keyword means that Strategy can not be inherited from
class Strategy final : public client::Handler {
/*
* Strategy inherits from client::Handler : https://roq-trading.com/docs/reference/development/roq-api/client/#handler
* client::Handler is used to implement handlers for update events
* Consists of a number of virtual public overloaded operator() functions that each take a different Event type
* (a virtual function implemented in the base class can be overridden in the subclass. good practice to mark the overwritten function with keyword override)
* Here, we implement the operator() function for handling TopOfBook Events
 */
 public:
  // constructor - Strategy is initialised with a Dispatcher
  explicit Strategy(client::Dispatcher &);  // explicit keyword disables implicit construction ie must be created with Strategy myStrategy(client::Dispatcher &) - bit of a safety feature

  Strategy(Strategy &&) = default;  // default constructor
  Strategy(const Strategy &) = delete;  // remove this constructor

 protected:
  void operator()(const Event<TopOfBook> &) override;

 private:
  client::Dispatcher &dispatcher_;  // Dispatcher is used to manage client requests
  zmq::Context context_;
  zmq::Socket socket_;
};

}  // namespace zeromq
}  // namespace samples
}  // namespace roq
