#pragma once

auto socketListenBind() -> int;

auto server_accept(int serverfd) -> int;

void show_info(int connfd);

void setNonBlocking(int fileDescriptor);

void setBlock(int fileDescriptor);
