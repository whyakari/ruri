// SPDX-License-Identifier: MIT
/*
 *
 * This file is part of ruri, with ABSOLUTELY NO WARRANTY.
 *
 * MIT License
 *
 * Copyright (c) 2022-2024 Moe-hacker
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
#include "include/ruri.h"
// Show some extra info when segfault.
static void sighandle(int sig)
{
	signal(sig, SIG_DFL);
	int clifd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
	char buf[1024];
	ssize_t bufsize = read(clifd, buf, sizeof(buf));
	cfprintf(stderr, "{base}");
	cfprintf(stderr, "{base}%s\n", "  .^.   .^.");
	cfprintf(stderr, "{base}%s\n", "  /⋀\\_ﾉ_/⋀\\");
	cfprintf(stderr, "{base}%s\n", " /ﾉｿﾉ\\ﾉｿ丶)|");
	cfprintf(stderr, "{base}%s\n", " ﾙﾘﾘ >  x )ﾘ");
	cfprintf(stderr, "{base}%s\n", "ﾉノ㇏  ^ ﾉ|ﾉ");
	cfprintf(stderr, "{base}%s\n", "      ⠁⠁");
	cfprintf(stderr, "{base}%s\n", "RURI ERROR MESSAGE");
	cfprintf(stderr, "{base}Seems that it's time to abort.\n");
	cfprintf(stderr, "{base}SIG: %d\n", sig);
	cfprintf(stderr, "{base}UID: %u\n", getuid());
	cfprintf(stderr, "{base}PID: %d\n", getpid());
	cfprintf(stderr, "{base}CLI: ");
	for (ssize_t i = 0; i < bufsize - 1; i++) {
		if (buf[i] == '\0') {
			fputc(' ', stderr);
		} else {
			fputc(buf[i], stderr);
		}
	}
	cfprintf(stderr, "{base}\nThis message might caused by an internal error.\n");
	cfprintf(stderr, "{base}If you think something is wrong, please report at:\n");
	cfprintf(stderr, "\033[4m{base}%s{clear}\n\n", "https://github.com/Moe-hacker/ruri/issues");
}
// Catch coredump signal.
void register_signal(void)
{
	signal(SIGABRT, sighandle);
	signal(SIGBUS, sighandle);
	signal(SIGFPE, sighandle);
	signal(SIGILL, sighandle);
	signal(SIGQUIT, sighandle);
	signal(SIGSEGV, sighandle);
	signal(SIGSYS, sighandle);
	signal(SIGTRAP, sighandle);
	signal(SIGXCPU, sighandle);
	signal(SIGXFSZ, sighandle);
}
