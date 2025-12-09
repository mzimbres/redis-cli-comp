package main

import (
  "context"
  "github.com/redis/rueidis"
)

const channel = "channel"
const payload = "payload"
const uds = "/run/redis/redis-server.sock"
const pings = 5
const sessions = 1000
const repeat = 5000

// TODO: This does not work with Unix Domain sockets.
func main() {
  client, err := rueidis.NewClient(rueidis.ClientOption{InitAddress: []string{"127.0.0.1:6379"}})
  if err != nil {
    panic(err)
  }
  defer client.Close()

  ctx := context.Background()

  err = client.Do(ctx, client.B().Ping().Build()).Error()
  if err != nil {
    panic(err)
  }
}
