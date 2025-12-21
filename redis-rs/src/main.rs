use std::sync::Arc;
use redis::{aio::MultiplexedConnection, RedisResult};

const ADDR: &str = "redis://127.0.0.1/?protocol=resp3";
const CHANNEL: &str = "channel";
const PAYLOAD: &str = "payload";
const RECV_CAPACITY: usize = 256;
const PINGS: usize = 5;
const SESSIONS: usize = 1000;
const REPEAT: usize = 10000;

// Number of events expected 
const EXPECTED_PUSHES: usize = SESSIONS * REPEAT;

fn make_req() -> redis::Pipeline {

    let mut cmds = redis::pipe();
    for _j in 0..PINGS {
        cmds.cmd("PING").ignore();
    }

    cmds.cmd("PUBLISH").arg(CHANNEL).arg(PAYLOAD).ignore();
    cmds
}

async fn session(mut con: MultiplexedConnection, cmds: Arc<redis::Pipeline>) -> RedisResult<()> {

    for _i in 0..REPEAT {
        cmds.exec_async(&mut con).await?;
    }

    Ok(())
}

#[tokio::main]
async fn main() {
    let client = redis::Client::open(ADDR).unwrap();

    let (tx, mut rx) = tokio::sync::mpsc::unbounded_channel();
    let config = redis::AsyncConnectionConfig::new().set_push_sender(tx);
    let mut con = client.get_multiplexed_async_connection_with_config(&config).await.unwrap();
    con.subscribe(CHANNEL).await.unwrap();

    let _ = rx.recv().await.unwrap();

    let cmds = Arc::new(make_req());
    for _i in 0..SESSIONS {
        tokio::spawn(session(con.clone(), cmds.clone()));
    }
    
    let buffer: &mut Vec<redis::PushInfo> = &mut Vec::with_capacity(RECV_CAPACITY);

    let mut n: usize = 0;
    while n < EXPECTED_PUSHES {
        let total_msgs = rx.recv_many(buffer, RECV_CAPACITY).await;
        n += total_msgs;
        buffer.clear();
    }
}
