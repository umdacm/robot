# Remote Robot Access

```
 +---------------------------+
 |Raspberry Pi: Proxy Server |
 |---------------------------|
 |robot.net.chandlerswift.com|
 | * SSH: 22 (user: pi)      |
 |   - Public Key Auth       |
 | * HTTP/S: 80/443          |
 | * pi pwd: 'password'      |
 +---------------------------+
               | 10.0.0.1
               |
               |
               v 10.0.0.2
 +---------------------------+
 |           Robot           |
 |---------------------------|
 | * SSH: 22 (user mobility) |
 |   - Public Key Auth       |
 | * Camera server port 10000|
 | * Drive server port 10001 |
 | * mobility pwd: 'password'|
 +---------------------------+
```

The computer built into the robot has an ethernet connection, which is
connected to a Raspberry Pi in Bridge mode. The Raspberry Pi is set up to be
connected to UMD's `eduroam` wireless network, which provides a public IP
address (otherwise, 22, 80, and 443 would need to be forwarded).
