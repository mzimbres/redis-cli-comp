# Redis client comparison

_Performance_ is Redis most advertized selling point. As an
_in-memory_ database it provides low-latency solutions for
backend typical needs: caching, queuing, rate limiting etc.

The Redis server however is only one side of the equation,
clients on the other end must be capable of using the server
efficiently for the performance of the system as a whole to
be great.

In this article we profiled the performance of some of the
most popular clients by simulating the scenario where Redis
is mostly used: internet facing servers (usually HTTP) that
serve connections concurrently while receiving server pushes
e.g. pubsub events. The benchmark consists of

  1. starting multiple sessions that issue commands in a loop.

  2. subscribing to a channel to receive pubsub events.

The following clients have been considered

Client                                                | UIA     | Stars   | Contributors | Age   | Lnaguage 
------------------------------------------------------|---------|---------|--------------|------|---------
[boost-redis](https://github.com/boostorg/redis)      |      64 |     270 |           14 |     6 | C++
[go-redis](https://github.com/redis/go-redis)         |    1090 |   21800 |          400 |    13 | Go
[rueidis](https://github.com/redis/rueidis)           |     174 |    2900 |          132 |     5 | Go
[redis-rs](https://github.com/redis-rs/redis-rs)      |     495 |    4100 |          261 |    12 | Rust
[redis++](https://github.com/sewenew/redis-plus-plus) |     327 |    1900 |           44 |     8 | C++
[fred-rs](https://github.com/aembke/fred.rs)*         |      94 |     505 |            1 |     4 | Rust

Where the abbreviation UIA used above refers to the unique number of
github issue authors (_Unique Issue Authors_)

* The `fred-rs` benchmark was aborted due to apparent bug or
  misuse of the library, which is being clarified in
  [this](https://github.com/aembke/fred.rs/issues/367)
  issue.

## Results

The parameters used in the simulation result in the following number
of requests, responses and pushes

Type        | Counter | Bytes (Mb)
------------|---------|-----------
Requests    |   60M   | 1130
Responses   |   60M   |  390
Pushes      |   10M   |  430

### Time

Client              | Time (s)
--------------------|---------
boost-redis-asio-co | 
boost-redis-asio-cb | 25.58
boost-redis-corosio |      
redis-rs            | 43.30
rueidis             |  
go-redis            |  

### CPU

Client              | user(%) | sys(%)
--------------------|---------|-------
boost-redis-asio-co |   74.06 |   3.68
boost-redis-asio-cb |   50.48 |   3.45
boost-redis-corosio |         |       
redis-rs            |  207.31 |  73.77
rueidis             |  219.68 |  60.00
go-redis            |  160.24 | 104.15

### Threads

Client              | threads | fd-nr
--------------------|---------|-------
boost-redis-asio-co |       2 |    7
boost-redis-asio-cb |       2 |    7
boost-redis-corosio |         |     
redis-rs            |       9 |   10
rueidis             |      12 |    6
go-redis            |      13 | 1001

### Context switches

Client              | context switches/s
--------------------|-------------------
boost-redis-asio-co |      1278.12
boost-redis-asio-cb |      1529.03
boost-redis-corosio |             
redis-rs            |     89826.72
rueidis             |      3882.10
go-redis            |      3050.94

### Cache and Branch misses

Client              | cache-misses  | branch-misses
--------------------|---------------|-------------
boost-redis-asio-co |   186,941,741 |   511,379,762
boost-redis-asio-cb |    91,815,535 |   264,195,103
boost-redis-corosio |               |              
redis-rs            | 1,067,373,045 | 3,489,504,327
rueidis             | 3,030,632,185 | 1,843,635,447
go-redis            | 5,644,549,412 | 2,843,844,568

