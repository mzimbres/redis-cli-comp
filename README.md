# Redis client comparison

The goal of this project is to compare the performance of Redis
clients in multiple languages. Although there is no single benchmark
that represents best all use cases a large subset of client usage are
internet facing (HTTP) servers that have multiple user connections.

Server sent pushes is another feature that clients have to deal with
e.g. when Redis is used as message broken pubsub and multiple other
forms of pushes.

We have used this as a base to define a task to be implemented by each
client. The benchamark app will implement the following

  1. Start multiple independent sessions that loop sending `PING`s and one `PUBLISH` command.

  2. Subscribe to the channel where messages are published and consume them

In 1. we use the PING command because it is very low weight for the
server i.e. we want to benchmark clients and not the server.

## Metrics

The time spent to execute the task is the best single number we can
come up with to compare clients. There are however some things we have
to watch closely though

  - We should make sure that the setup is not IO bound otherwise the
    numbers won't convey information useful for comparison. This can
    be avoided by communicating through a fast link such as a Unix
    Socket [1]. This will help reducing the time the process spends in
    kernel space which is common to all apps since they are expected
    to communicated the exact some data.

  - If a client cannot saturate the CPU it might be because the Redis
    server is saturated. When two clients need the same amount of time
    to execute the task, the one with the lowest CPU usage is obviously
    the most efficient.

  - It should be possible to saturate the CPU when the setup is
    neither IO bound nor the Server is saturated. A client that fails
    to do that has some form of contention which is a bad thing for
    performance. Given the server only performs light weight
    operations it is expected that the client will saturate the CPU
    before the server.

  - TODO: Talk about multiplexing and pipelining.

## TODO

  - Compare time to compile and number of lines of code.
  - Test redigo.

## Results

time (s)    | real    | user    | sys
------------|---------|---------|-------
boost-redis |  20.813 |  18.134 |   1.060
go-redis    | 121.212 | 215.867 | 102.258
rueidis     |  55.562 | 132.746 |  34.274
redis-rs    |  95.135 | 187.079 |  66.864

Popularity  | Stars   | UIA     | Age   | Birth   
------------|---------|---------|-------|---------
boost-redis |     270 |      64 |     6 | Nov 2019
go-redis    |   21800 |    1090 |    13 | Jul 2012
rueidis     |    2900 |     174 |     5 | Sep 2021
redis-rs    |    4100 |     495 |    12 | Dec 2013
redis++     |    1900 |     327 |     8 | Dec 2017

UIA: Number of unique issue authors.


[1] On the system I tested has a thoughput of 5Gb/s TODO: Write a bash script that shows that.

```sh
awk 'NR==1 { min=$2 } FNR==NR{min=($2+0<min)?$2:min;next} {print $1,$2/min}' foo.txt foo.txt
```
