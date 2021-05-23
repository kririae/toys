enum Config {
  LocalConfig,
  ServerConfig,
}

struct LocalConfig {
  Type: String,
  RemoteAddr: String,
  RemotePort: u16,
}

struct ServerConfig {
  // --snip--
}

impl Config {
  fn load(path: &str) {
    
  }
}
