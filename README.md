# Redis client comparison

The most important Redis selling point is performance. As an
_in-memory_ database it promises a low-latency solution for typical
backend needs: caching, queuing, rate limiting etc.

The Redis server however is only one side of the equation. It does not
matter how fast Redis reacts to a request if the clients on other end
cannot make proper use of that performance and push the server to its
limits. To assess the performance profile of Redis clients we need a
benchmark that is representative of how Redis is used in production
environment.

It is safe to say that the majority of apps using Redis are internet
facing servers (mostly HTTP) that serve connections concurrently while
perhaps receiving some form of pubsub events, for example

  1. Start multiple sessions that loop sending multiple `PING`s and
     one `PUBLISH` command.

  2. Subscribe to the channel where messages are published in 1. and
     consume them.

where

  - sessions above refer to coroutine (c++), goroutine (go), tasks (rust).

  - The specific commands sent by each session is not important for
    the purpose of the benchmark. `PING` commands good choice here
    because they are lightweight for both the server and client.

## Clients tested

The benchmark laid out above was implement in the most popular C++, Go
and Rust clients. The table below provides provides useful metrics
about the popularity of each cleint

Popularity  | Stars   | UIA     | Contributors | Age   | Birth   
------------|---------|---------|--------------|-------|---------
boost-redis |     270 |      64 |           14 |     6 | Nov 2019
go-redis    |   21800 |    1090 |          400 |    13 | Jul 2012
rueidis     |    2900 |     174 |          132 |     5 | Sep 2021
redis-rs    |    4100 |     495 |          261 |    12 | Dec 2013
redis++     |    1900 |     327 |           44 |     8 | Dec 2017
fred-rs     |     505 |      94 |            1 |     4 | Aug 2021
redigo      |    9900 |     375 |           80 |    13 | Apr 2012

## Results

The raw results data is summarised below

client      | real(s) | user(s) | sys(s) | CPU(%)
------------|---------|---------|--------|-------
boost-redis |  31.74  |   23.48 |   1.32 |    78%
rueidis     |  66.07  |  145.36 |  38.73 |   278%
redis-rs    | 103.66  |  217.51 |  76.16 |   283%
go-redis    | 114.31  |  218.65 | 103.64 |   281%

where UIA is the number of unique issue authors of the respective
repository. For comparison purposes it is also useful to look at the
normalied values

client      | real    | user    | sys    | CPU   
------------|---------|---------|--------|-------
boost-redis |   1.000 |   1.000 |  1.000 | 1.000
rueidis     |   2.082 |   6.191 | 29.341 | 3.564
redis-rs    |   3.266 |   9.264 | 57.697 | 3.628
go-redis    |   3.601 |   9.312 | 78.515 | 3.603

## Conclusion

The intrinsic performance difference of the languages used to implemet
the clients cannot account for the performance differece in the
benchmarks. The differences is mostly due to design problems.

## TODO

  - Compare time to compile and number of lines of code.
  - Fred bug report: It does not receive all events.
  - Test redigo.

