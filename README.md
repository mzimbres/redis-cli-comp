# Redis client comparison

The goal is to compare the performance of different Redis client in
multiple languages and drivers. The algorith used is

  - Starts `n` concurrent sessions
  - Subscribe to events from a predefined pubsub channel.
  - Let each task loop sending the following commands

    - `N` `PING`s
    -  One `PUBLISH` on the predefined channel.
  
  - Read and discard the pushes.

Metrics used to compare the clients

  1. Client and server CPU usage. A good client should drive server
     CPU as high as possible while keeping it own CPU as low as
     possible.

  2. It should be possible for the client to saturate the CPU if it is
     not IO bound.

  3. The more time the client spends executing in the system (as
     opposed to user space) the better.

