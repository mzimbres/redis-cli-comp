use futures::prelude::*;
use redis::{aio::MultiplexedConnection, RedisResult};

//const PINGS: i64 = 5;
const SESSIONS: usize = 1000;

async fn session(con: &MultiplexedConnection) -> RedisResult<()> {
    let mut con = con.clone();

    for _i in 1..SESSIONS {
        redis::cmd("PING").exec_async(&mut con).await?;
        redis::cmd("PING").exec_async(&mut con).await?;
        redis::cmd("PING").exec_async(&mut con).await?;
        redis::cmd("PING").exec_async(&mut con).await?;
        redis::cmd("PING").exec_async(&mut con).await?
    }

    // TODO: Remove this
    redis::cmd("PING").exec_async(&mut con).await
}

#[tokio::main]
async fn main() {
    let client = redis::Client::open("redis://127.0.0.1/").unwrap();

    let con = client.get_multiplexed_async_connection().await.unwrap();

    let cmds = (1..SESSIONS).map(|_i| session(&con));
    let result = future::try_join_all(cmds).await.unwrap();

    assert_eq!(SESSIONS, result.len());
}
