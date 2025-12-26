use fred::prelude::*;

const CHANNEL: &str = "channel";
const PAYLOAD: &str = "payload";
const POOL_SIZE: usize = 256;
const PINGS: usize = 5;
const SESSIONS: usize = 1000;
const REPEAT: usize = 1000;

// Number of events expected 
const EXPECTED_PUSHES: usize = SESSIONS * REPEAT;

async fn session(pool: Pool) -> Result<(), fred::error::Error> {
    for _i in 0..REPEAT {
        let client = pool.next();
        let pipe = client.pipeline();
        for _j in 0..PINGS {
            let _: () = pipe.ping(None).await?;
        }
        let _: () = pipe.publish(CHANNEL, PAYLOAD).await?;
        let _: () = pipe.all().await?;
    }

    Ok(())
}

#[tokio::main]
async fn main() -> Result<(), Error> {
    // ====================================================
    // Separate connection for the subscriber.
    let sub = Builder::default_centralized()
    .with_performance_config(|config| {
        config.broadcast_channel_capacity = 256;
    })
    .build()?;
    sub.init().await?;

    // Subscribe before starting to publish.
    let _ = sub.subscribe(CHANNEL).await?;

    // ====================================================
    // Spawn publisher tasks.
    let pool = Builder::default_centralized().build_pool(POOL_SIZE)?;
    pool.init().await?;
    for _i in 0..SESSIONS {
        tokio::spawn(session(pool.clone()));
    }

    // ====================================================
    // Loop receiving events.
    let mut rx = sub.message_rx();
    let mut n: usize = 0;
    while n < EXPECTED_PUSHES {
        let _ = rx.recv().await;
        n += 1;
        println!("finish {}/{}", n, EXPECTED_PUSHES);
    }

    Ok(())
}
