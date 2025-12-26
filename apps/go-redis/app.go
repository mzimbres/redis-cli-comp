package main

import (
    "context"
    "github.com/redis/go-redis/v9"
    "fmt"
    "sync"
)

// Parameters
const channel = "channel"
const payload = "payload"
const uds = "/run/redis/redis-server.sock"
const pings = 5
const sessions = 1000
const repeat = 10000

// Number of expected events
const expected_events = sessions * repeat;

var ctx = context.Background()

func session(ops *redis.Options) {
	rdb := redis.NewClient(ops)

	for range repeat {
		pipe := rdb.Pipeline()
		for range pings {
			pipe.Ping(ctx)
		}

		pipe.Publish(ctx, channel, payload)

		_, err := pipe.Exec(ctx)
		if err != nil {
			fmt.Println("ERROR", err)
			panic(err)
		}
	}

	rdb.Close()
}

func subscribe(ops redis.Options) <-chan *redis.Message {

	client := redis.NewClient(&ops)
	sub := client.Subscribe(ctx, channel)

	// Wait for the message to make sure no message will be lost
	// when we start to publish in the sessions.
	iface, err := sub.Receive(ctx)
	if err != nil {
		panic(err)
	}

	fmt.Println(iface)

	return sub.Channel()
}

func receive(ch <-chan *redis.Message) {
	for i := 0; i < expected_events; i++ {
		<-ch
	}
}

func main() {
	fmt.Println("Number of events expected: ", expected_events)

	ops := redis.Options{
		Addr:     uds,
		Password: "",
		DB:       0,
		Protocol: 3,
	}

	var wg sync.WaitGroup

	ch := subscribe(ops)

	// Spawn the pubsub receiver
	wg.Add(1)
	go func() {
		defer wg.Done()
		receive(ch)
	}()

	// Spawn the sessions
	for j := 0; j < sessions; j++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			session(&ops)
		}()
	}

	wg.Wait()
}
