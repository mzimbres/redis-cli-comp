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
------------------------------------------------------|---------|---------|--------------|-------|---------
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

## Performance Results

The parameters used in the simulation result in the following number
of requests, responses and pushes

Type        | Counter | Bytes (Mb)
------------|---------|-----------
Requests    |   60M   | 1130
Responses   |   60M   |  390
Pushes      |   10M   |  430

### Time

Client              | Time (s/s)
--------------------|---------
boost-redis-asio-co |  2.2
boost-redis-asio-cb |  1.3
boost-redis-corosio |  1   
redis-rs            |  4.7
rueidis             |  -
go-redis            | 27.6

The table above provide the times normalized to the fastest client which was
boost-redis running on corosio.  The value has been computed by multiplying the
wall time with the percentage of CPU usage by the client.

### CPU

Client              | user(%) | sys(%)
--------------------|---------|-------
boost-redis-asio-co |   65.0  |    3.3
boost-redis-asio-cb |   46.2  |    2.8
boost-redis-corosio |   32.5  |    6.2
redis-rs            |   98.2  |    1.6
rueidis             |    -    |     -     
go-redis            |  174.0  |  105.1

### Threads

Client              | threads | fd-nr
--------------------|---------|-------
boost-redis-asio-co |    2    |    7
boost-redis-asio-cb |    2    |    7
boost-redis-corosio |    3    |    7
redis-rs            |    1    |   10
rueidis             |    -    |    -
go-redis            |   24    | 1006

### Context switches

Client              | context switches/s
--------------------|-------------------
boost-redis-asio-co |   2733.56
boost-redis-asio-cb |   2861.21
boost-redis-corosio |   3325.44
redis-rs            |     25.85
rueidis             |          
go-redis            |   3113.71

### Cache and Branch misses

Client              | cache-misses  | branch-misses
--------------------|---------------|-------------
boost-redis-asio-co |    16,808,299 |    84,612,336             
boost-redis-asio-cb |     6,979,958 |    56,653,319             
boost-redis-corosio |    19,369,915 |    75,381,092             
redis-rs            |       736,951 |   302,865,130             
rueidis             |          -    |      -                    
go-redis            | 1,216,462,587 | 1,666,424,709             

## Build time Results

### App build time

Time needed to build the app with one core after touching the implementation
file 

Client              | Time
--------------------|--------------
boost-redis-asio-co | 3.59
boost-redis-asio-cb | 8.95
boost-redis-corosio | 1.80
redis-rs            | 0.16
fred-rs             | 0.19
go-redis            | 0.04
rueidis             | 0.04

### Client build time

Time needed to build the client with one core after touching the implementation
file 

Client              | Time
--------------------|--------------
