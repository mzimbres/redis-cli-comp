# redis-cli-comp

Compare Redis client performance. Metrics we use

  1. Client and server CPU usage. A good client should drive server
     CPU as high as possible.

  2. It should be possible for the client to saturate the CPU if it is
     not IO bound.

  3. The more time the client spends executing in the system (as
     opposed to user space) the better.

We compare the app tasks in multiple languages

  - App starts `n` concurrent sessions and subscribes to events from a
    predefined pubsub channel.

  - Each task loops sending the following commands

    - `a` `PING`s
    -  One `PUBLISH` on a predefined channel.
  
  - Read and discard the pushes.
