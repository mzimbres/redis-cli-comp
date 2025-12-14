package main

import (
	"context"
	"github.com/redis/rueidis"
	"sync"
	"fmt"
)

const channel = "channel"
const payload = "payload"
const addr = "127.0.0.1:6379" // "/run/redis/redis-server.sock"
const pings = 5
const sessions = 1000
const repeat = 10000

// Number of expected events
const expected_events = sessions * repeat;

// The implementation below is somehow unreliable. I did not find a
// way to start the goroutines after the subscription confirmation
// arrived.
func main() {
	fmt.Println("Number of events expected: ", expected_events)

	client, err := rueidis.NewClient(rueidis.ClientOption{InitAddress: []string{addr}})
	if err != nil {
		panic(err)
	}

	defer client.Close()

	ctx := context.Background()
	ping := client.B().Ping().Build().Pin()
	pub := client.B().Publish().Channel(channel).Message(payload).Build().Pin()

	var wg sync.WaitGroup

	for j := 0; j < sessions; j++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < repeat; i++ {
				for i := 0; i < pings; i++ {
					err = client.Do(ctx, ping).Error()
					if err != nil {
						panic(err)
					}
				}
				err = client.Do(ctx, pub).Error()
				if err != nil {
					panic(err)
				}
			}
		}()
	}

	sub := client.B().Subscribe().Channel(channel).Build()

	i := 0
	err = client.Receive(ctx, sub, func(msg rueidis.PubSubMessage) {
		//fmt.Printf("%s\n", msg)
		i++
		if i == expected_events {
			client.Close()
		}
	})

	if err != nil {
		panic(err)
	}

	wg.Wait()
}
