# Socket Server Infrastructure

Yet another Socket Server infrastructure with "Unix Domain Socket" (UDS) (AF_UNIX/SOCK_STREAM) and "TCP" (AF_INET/SOCK_STREAM) socket implementation. (UPD (AF_INET/SOCK_DGRAM) is not completely finished yet.)

You can develop your own high-performance Client/Server implementation and easily switch between "Unix Domain Socket" or "TCP" implementation.

## Table of Contents
- [Socket Server Infrastructure](#socket-server-infrastructure)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Installing](#installing)
  - [Usage](#usage)
    - [Server](#server)
    - [Client](#client)
  - [Features](#features)

## Introduction
This project demonstrates a Socket Server infrastructure using both Unix Domain Sockets (UDS) and TCP sockets. It provides an easy way to develop high-performance Client/Server implementations that can switch between UDS and TCP seamlessly.

## Installation
Instructions on how to install and set up your project. Include any prerequisites that are necessary for running the project.

### Prerequisites
- List any software or libraries that need to be installed.

### Installing
1. Clone the repository:
    ```sh
    git clone https://github.com/cangursu/SocketServer.git
    ```
2. Navigate to the project directory:
    ```sh
    cd SocketServer
    mkdir -p build
    cd build
    ```
3. Compile the project:
    ```sh
    cmake .. && cmake --build .
    ```

## Usage
You can find examples in the `server` and `client` folders for Server and Client usage.

### Server
The `server/src/main.cpp` file demonstrates a `SocketServer` implementation for TCP and UDS. The `EchoServer` is a sample Server implementation that receives a string from clients and prints it to the STDOUT stream. You can see how to use TCP or UDS `SocketServer` with `FDBASE_UDS` and `FDBASE_TCP` preprocessors.

1. Execute the compiled server binary:
    ```sh
    ./bin/SocketServerTCP
    ./bin/SocketServerUDS
    ```

### Client
The `client/src/main.cpp` file demonstrates a `SocketClient` implementation. This example shows you how to switch between TCP and UDS via `FDBASE_TCP` or `FDBASE_UDS`, which is quite similar to `EchoServer`. The `EchoClient` is your client implementation, and in this example, the client sends a string (without a null terminator) to the server but does not receive anything.

1. Execute the compiled client binary:
    ```sh
    ./bin/SocketServerTCP
    ./bin/SocketServerUDS
    ```

## Features
- High-performance Client/Server implementation.
- Easy switching between Unix Domain Socket (UDS) and TCP implementations.
- Example implementations provided for both server and client.
