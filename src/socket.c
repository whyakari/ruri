// SPDX-License-Identifier: MIT
/*
 *
 * This file is part of ruri, with ABSOLUTELY NO WARRANTY.
 *
 * MIT License
 *
 * Copyright (c) 2022-2023 Moe-hacker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */
#include "ruri.h"
// For daemon.
ssize_t send_msg_daemon(char *msg, struct sockaddr_un addr, int sockfd)
{
  /*
   * It will accept a new connection and write msg to socket.
   * Although the returned value has never been used, it will return the number written.
   */
#ifdef __RURI_DEV__
  printf("%s%s\033[0m\n", "\033[1;38;2;254;228;208mDaemon send msg: \033[1;38;2;152;245;225m", msg);
#endif
  unsigned int size = sizeof(addr);
  // Accept a connection.
  int sock_new = accept4(sockfd, (struct sockaddr *)&addr, &size, SOCK_CLOEXEC);
  // Set timeout duration.
  struct timeval timeout = {3, 0};
  setsockopt(sock_new, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  setsockopt(sock_new, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  // Send messages.
  return write(sock_new, msg, strlen(msg));
}
// For client.
ssize_t send_msg_client(char *msg, struct sockaddr_un addr)
{
  /*
   * It will send msg to socket and quit.
   * Although the returned value has never been used, it will return the number written.
   */
#ifdef __RURI_DEV__
  printf("%s%s\033[0m\n", "\033[1;38;2;254;228;208mClient send msg: \033[1;38;2;152;245;225m", msg);
#endif
  int sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (sockfd < 0)
  {
    perror("socket");
    return -1;
  }
  // Set timeout duration.
  struct timeval timeout = {0, 100};
  setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  // Connect to daemon.
  if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    return -1;
  }
  // Send messages.
  return send(sockfd, msg, strlen(msg), 0);
}
// For daemon, return the messages have been read.
ssize_t read_msg_daemon(char *buf, struct sockaddr_un addr, int sockfd)
{
  /*
   * It will read a message from socket,
   * and write the messages have been read to buf.
   * Return the same value as read().
   */
  // Clear buf.
  memset(buf, '\000', strlen(buf));
  ssize_t ret = 0;
  unsigned int size = sizeof(addr);
  // Accept a connection.
  int sock_new = accept4(sockfd, (struct sockaddr *)&addr, &size, SOCK_CLOEXEC);
  // Set timeout duration.
  struct timeval timeout = {3, 0};
  setsockopt(sock_new, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  setsockopt(sock_new, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  // Read messages.
  ret = read(sock_new, buf, PATH_MAX);
  if (ret == -1)
  {
    buf[0] = '\000';
  }
  close(sock_new);
#ifdef __RURI_DEV__
  if (ret != -1)
  {
    printf("%s%s\n\033[0m", "\033[1;38;2;254;228;208mDaemon read msg: \033[1;38;2;152;245;225m", buf);
  }
  else
  {
    printf("%s\n\033[0m", "\033[1;38;2;254;228;208mDaemon read msg: \033[1;38;2;152;245;225mNULL");
  }
#endif
  return ret;
}
// For client, return the messages have been read.
ssize_t read_msg_client(char *buf, struct sockaddr_un addr)
{
  /*
   * It will read a message from socket,
   * and write the messages have been read to buf.
   * Return the same value as read().
   */
  // Clear buf.
  memset(buf, '\000', strlen(buf));
  ssize_t ret = 0;
  int sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (sockfd < 0)
  {
    perror("socket");
  }
  // Set timeout duration.
  struct timeval timeout = {1, 100};
  setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  // Connect to daemon.
  if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    return -1;
  }
  // Read messages.
  ret = read(sockfd, buf, PATH_MAX);
  if (ret == -1)
  {
    buf[0] = '\000';
  }
  close(sockfd);
#ifdef __RURI_DEV__
  if (ret != -1)
  {
    printf("%s%s\033[0m\n", "\033[1;38;2;254;228;208mClient read msg: \033[1;38;2;152;245;225m", buf);
  }
  else
  {
    printf("%s\033[0m\n", "\033[1;38;2;254;228;208mClient read msg: \033[1;38;2;152;245;225mNULL");
  }
#endif
  return ret;
}
// Connect to daemon.
int connect_to_daemon(struct sockaddr_un *addr)
{
  /*
   * Set socket address and check if daemon is running.
   * Return -1 if error.
   */
  // Set socket address.
  addr->sun_family = AF_UNIX;
  // In termux, $TMPDIR is not /tmp, so we get $TMPDIR for tmp path.
  char *tmpdir = getenv("TMPDIR");
  if ((tmpdir == NULL) || (strcmp(tmpdir, "") == 0))
  {
    tmpdir = "/tmp";
  }
  char socket_path[PATH_MAX] = {0};
  strcat(socket_path, tmpdir);
  strcat(socket_path, "/");
  strcat(socket_path, SOCKET_FILE);
  strcpy(addr->sun_path, socket_path);
  // Try to connect to socket file and check if it's created by ruri daemon.
  send_msg_client(FROM_CLIENT__TEST_MESSAGE, *addr);
  char msg[MSG_BUF_SIZE] = {'\000'};
  // Clear buf.
  memset(msg, '\000', MSG_BUF_SIZE);
  read_msg_client(msg, *addr);
  if (strcmp(FROM_DAEMON__TEST_MESSAGE, msg) != 0)
  {
    return -1;
  }
  return 0;
}