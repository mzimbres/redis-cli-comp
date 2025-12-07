package main

import (
    "context"
    "github.com/redis/go-redis/v9"
    "fmt"
    "sync"
)

var ctx = context.Background()

func session(ops redis.Options) {

	rdb := redis.NewClient(&ops)


	for {
		pipe := rdb.Pipeline()

		pipe.Ping(ctx)
		pipe.Ping(ctx)
		pipe.Ping(ctx)
		_, err := pipe.Exec(ctx)
		if err != nil {
			fmt.Println("ERROR", err)
			panic(err)
		}
	}

	rdb.Close()
}

func main() {
	ops := redis.Options {
		Addr:     "/run/redis/redis-server.sock",
		Password: "",
		DB:       0,
		Protocol: 3,
	}

	var wg sync.WaitGroup

	// TODO: Update go and use wg.Add.
	for j := 0; j < 10; j++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			session(ops)
		}()
	}

	wg.Wait()
}
