# High-Performance HTTP Web Server

## Project Overview
A robust, single-threaded C++ web server implementation designed for FreeBSD-based operating systems (e.g., macOS), utilizing KQueue for efficient non-blocking I/O and showcasing advanced network programming techniques.

## Key Features
- Full HTTP/1.1 protocol implementation
- Single-threaded architecture
- KQueue-based event handling (macOS specific)
- Non-blocking I/O design
- Configurable server architecture
- Dynamic CGI script execution

## Platform Compatibility
- **Current Support**: macOS
- **Containerized Version**: Coming soon

## Prerequisites
- C++98 compatible compiler
- macOS operating system
- Build tools (make)

## Configuration
Server behavior defined through a flexible configuration file, supporting:
- Custom port and host settings
- Server name configurations
- Route definitions
- Error page management
- CGI script paths

## Build
- For regular build: ```bash make ```
- For building with debugging mode: ```bash make debug ```

## Usage
```bash ./webserv [configuration_file] ``` or you can use the default provided config file ```bash ./webserv ```

## Technical Highlights
- KQueue event notification mechanism
- Efficient single-threaded connection management
- Low-overhead network programming
- Custom HTTP protocol parsing

## Performance Characteristics
- Minimal resource consumption
- Low-latency request processing (Successfully handled over 180 concurrent requests using tools like Apache Benchmark and Siege)
- Optimized for macOS environments

## Upcoming Developments
- Docker containerization
- Potential cross-platform compatibility

## License
Open-source project for educational and demonstration purposes.
