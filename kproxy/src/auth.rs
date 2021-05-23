fn handle_authorization(stream: &TcpStream) -> bool {
  let addr = stream.local_addr().unwrap().ip();
  if let IpAddr::V6(_) = addr {
    panic!("Cannot handle ipv6 connections");
  }

  // TODO: HashMap
  match addr {
    IpAddr::V4(v) => v == Ipv4Addr::new(127, 0, 0, 1),
    _ => false,
  }
}
