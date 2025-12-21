use redis::{aio::MultiplexedConnection, RedisResult};

const CHANNEL: &str = "channel";
const PAYLOAD: &str = "payload";
const PINGS: i64 = 5;
const SESSIONS: usize = 1000;
const REPEAT: usize = 10000;

// Number of events expected 
const EXPECTED_PUSHES: usize = 1 + SESSIONS * REPEAT;

async fn session(mut con: MultiplexedConnection) -> RedisResult<()> {

    for _i in 0..REPEAT {
        for _j in 0..PINGS {
            redis::cmd("PING").exec_async(&mut con).await?;
        }

        redis::cmd("PUBLISH")
            .arg(CHANNEL)
            .arg(PAYLOAD)
            .exec_async(&mut con).await?;
    }

    Ok(())
}

#[tokio::main]
async fn main() {
    let client = redis::Client::open("redis://127.0.0.1/?protocol=resp3").unwrap();

    let con = client.get_multiplexed_async_connection().await.unwrap();

    for _i in 0..SESSIONS {
        tokio::spawn(session(con.clone()));
    }

    let (tx, mut rx) = tokio::sync::mpsc::unbounded_channel();
    let config = redis::AsyncConnectionConfig::new().set_push_sender(tx);
    let mut con = client.get_multiplexed_async_connection_with_config(&config).await.unwrap();
    con.subscribe(CHANNEL).await.unwrap();
    
    for _i in 0..EXPECTED_PUSHES {
        let _ = rx.recv().await.unwrap();
    }
}
