/* Copyright (c) 2018-2022 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/redis/co_connection.hpp>
#include <boost/redis/config.hpp>

#include <boost/capy/ex/run_async.hpp>
#include <boost/capy/io_task.hpp>
#include <boost/capy/task.hpp>
#include <boost/capy/when_any.hpp>
#include <boost/corosio/io_context.hpp>

#include <exception>
#include <iostream>

// Parameters
constexpr char const* channel = "channel";
constexpr char const* payload = "payload";
constexpr std::size_t pings = 5;
constexpr std::size_t sessions = 1000;
constexpr std::size_t repeat = 30000;

// Number of events expected 
constexpr auto expected_pushes = 1 + sessions * repeat;

// Number of responses expected 
constexpr auto expected_resps = sessions * pings;

namespace capy = boost::capy;
using namespace boost::redis;
namespace corosio = boost::corosio;

capy::io_task<> run_request(std::shared_ptr<co_connection> conn)
{
   auto ex = co_await capy::this_coro::executor;

   request sub_req;
   sub_req.push("SUBSCRIBE", channel);
   auto [ec] = co_await conn->exec(sub_req);
   if (ec) {
      std::cout << "SUBSCRIBE error: " << ec << std::endl;
      exit(-1); // TODO: Exit cleanly.
   }

   auto session_req = std::make_shared<request>();
   for (std::size_t i = 0u; i < pings; ++i)
      session_req->push("PING");

   session_req->push("PUBLISH", channel, payload);

   for (auto i = 0u; i < sessions; ++i) {
      auto const session = [conn, session_req]() -> capy::task<> {
         for (auto i = 0u; i < repeat; ++i) {
            auto [ec] = co_await conn->exec(*session_req);
               if (ec) {
                  std::cout << "SUBSCRIBE error: " << ec << std::endl;
                  exit(-1); // TODO: Exit cleanly.
               }
         }
      };
      capy::run_async(ex)(session());
   }

   while (conn->get_usage().pushes_received < expected_pushes) {
      co_await conn->receive();
   }

   co_return {};
}

capy::task<void> co_main()
{
   auto ex = co_await capy::this_coro::executor;
   auto conn = std::make_shared<co_connection>(ex);
   co_await capy::when_any(run_request(conn), conn->run(config{}));
}

int main()
{
   // The I/O context, required for all I/O operations
   corosio::io_context ctx;

   // Schedules the main coroutine for execution
   capy::run_async(
      ctx.get_executor(),
      []() {
         // Runs when the main coroutine finishes normally
         std::cout << "Done\n";
      },
      [](std::exception_ptr exc) {
         // Runs when the main coroutine finishes with an exception
         try {
            std::rethrow_exception(exc);
         } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
         }
         exit(1);
      })(co_main());

   // Executes all pending work, including the main coroutine
   ctx.run();
}
