cmake_minimum_required(VERSION 3.0.0)
project(socket VERSION 0.1.0)


add_executable(server server.c protocol.c)
add_executable(client client.c)
