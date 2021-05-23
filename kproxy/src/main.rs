use env_logger::Env;
use log::{info, warn};
use std::env;
use std::{net::SocketAddr, sync::Arc};
use tokio::{
  io::{AsyncReadExt, AsyncWriteExt},
  net::{TcpListener, TcpStream},
  sync::Mutex,
};

#[tokio::main]
async fn main() -> ! {
  let args: Vec<String> = env::args().collect();

  println!("args: {:?}", args);

  // Init logger
  env_logger::Builder::from_env(Env::default().default_filter_or("info,debug")).init();

  let listener = TcpListener::bind("127.0.0.1:8080").await.unwrap();
  info!("Start listening at 127.0.0.1:8080");

  let cnt = Arc::new(Mutex::new(0));
  loop {
    let cnt = Arc::clone(&cnt);
    let (stream, addr) = listener.accept().await.unwrap();

    tokio::spawn(async move {
      process_conn(stream, addr).await;
      let mut lock = cnt.lock().await;
      *lock += 1;

      info!("Finished request: {}", *lock);
    });
  }
}

async fn process_conn(mut stream: TcpStream, addr: SocketAddr) {
  let mut buffer = [0; 4096];
  // let addr = stream.peer_addr().unwrap();
  log::info!("Handling connection from {}", addr);

  let result = stream.read_exact(&mut buffer[0..2]).await;
  if let Err(_) = result {
    warn!("Client responded with nothing");
    return;
  }

  assert_eq!(buffer[0], 0x05);
  let nmethods = buffer[1] as usize;

  // With enough information?
  stream
    .read_exact(&mut buffer[2..((2 + nmethods) as usize)])
    .await
    .unwrap();

  // Whatever, respond with passed.
  stream.write(&[0x05, 0x00]).await.unwrap();
  stream.flush().await.unwrap();

  // The second request
  let result = stream.read_exact(&mut buffer[0..4]).await;
  if let Err(_) = result {
    warn!("Client responded with nothing");
    return;
  }

  let cmd = buffer[1];
  assert_eq!(cmd, 0x01);
  let atyp = buffer[3];
  let target = match atyp {
    // ATYP
    0x01 => {
      info!("ATYP Request: ipv4");
      // ipv4
      log::error!("Do not support ipv4 request yet");
      return;
      // "127.0.0.1".to_string()
    }
    0x03 => {
      info!("ATYP Request: DOMAINNAME");
      stream.read_exact(&mut buffer[0..1]).await.unwrap();
      let len = buffer[0] as usize;

      // Read DOMAINNAME
      stream.read_exact(&mut buffer[0..(len + 2)]).await.unwrap();
      let domain = String::from_utf8_lossy(&buffer[..len]);
      let mut port: u16 = buffer[len] as u16;
      port = (port << 8) | (buffer[len + 1] as u16);
      info!("Resolved DOMAINNAME: {}:{}", domain, port);

      // for host in tokio::net::lookup_host(domain) {
      // info!("Found address: {}", host);
      // }

      format!("{}:{}", domain, port)
    }
    0x04 => {
      info!("ATYP Request: ipv6");

      log::error!("Do not support ipv6 request yet");
      return;
      // "::1".to_string()
    }
    _ => {
      panic!("ATYPE Request not supported");
    }
  };

  info!("Get target: {}", target);

  let remote = TcpStream::connect(target).await;

  if let Err(_) = remote {
    stream
      .write(&[0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
      .await
      .unwrap();
    warn!("Error reading remote");
    return;
  }

  let mut remote = remote.unwrap();

  // Fake result
  stream
    .write(&[0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    .await
    .unwrap();

  // Copy instream to ostream
  info!("Start relaying stream");
  let result = tokio::io::copy_bidirectional(&mut stream, &mut remote).await;

  if let Err(_) = result {
    warn!("Relay failed, finished");
  } else {
    info!("Finished relaying stream");
  }
}
