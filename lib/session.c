/*
 * Copyright (c) 2013, Kalopa Research Limited.  All rights reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2, or (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this product; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH LIMITED "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA RESEARCH LIMITED
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ABSTRACT
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>

#include "nehie.h"

#define FD_READ		1
#define FD_WRITE	2

#define DEFAULT_PORT	5000

/*
       int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);

       void FD_CLR(int fd, fd_set *set);
       int  FD_ISSET(int fd, fd_set *set);
       void FD_SET(int fd, fd_set *set);
       void FD_ZERO(fd_set *set);
*/



static	int	maxfds;
static	fd_set	mrdfds;
static	fd_set	mwrfds;

static	struct	session	*sfreelist;

static void	add_fd(int, int);

/*
 *
 */
void
session_init()
{
	maxfds = 0;
	FD_ZERO(&mrdfds);
	FD_ZERO(&mwrfds);
	sfreelist = NULL;
}

/*
 *
 */
struct session *
session_listen(char *uri)
{
	int fd, is_unix, len, port = DEFAULT_PORT;
	char *cp, *myhost;
	struct session *sp;
	struct sockaddr *sap;
	struct sockaddr_un sun;
	struct sockaddr_in sin;

	/*
	 * What kind of URI is it? Unix-domain sockets start with a /, and
	 * are the fully-qualified path of the socket. IP sockets have an
	 * IP address and an optional port. For example "0.0.0.0:5000".
	 */
	myhost = strdup(uri);
	if (*myhost == '/')
		is_unix = 1;
	else {
		is_unix = 0;
		if ((cp = strchr(myhost, ':')) != NULL) {
			*cp++ = '\0';
			port = atoi(cp);
		}
	}
	/*
	 * Get a socket.
	 */
	if ((fd = socket(is_unix ? AF_UNIX : AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("session_listen: socket");
		free(myhost);
		return(NULL);
	}
	if (is_unix) {
		memset(&sun, 0, sizeof(struct sockaddr_un));
		sun.sun_family = AF_UNIX;
		if (strlen(myhost) > PATH_MAX)
			myhost[PATH_MAX] = '\0';
		strcpy(sun.sun_path, myhost);
		sap = (struct sockaddr *)&sun;
		len = sizeof(sun);
	} else {
		memset(&sin, 0, sizeof(struct sockaddr_in));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = inet_addr(myhost);
		sin.sin_port = htons(port);
		sap = (struct sockaddr *)&sin;
		len = sizeof(sin);
	}
	free(myhost);
	if (bind(fd, sap, len) < 0) {
		perror("session_listen: bind");
		close(fd);
		return(NULL);
	}
	listen(fd, 2000);
	sp = session_alloc();
	sp->type = SESSION_MASTER;
	sp->state = SESSION_OPEN;
	sp->client_fd = fd;
	add_fd(fd, FD_READ);
	return(sp);
}

int
session_poll(struct session *sp)
{
	int n;
	fd_set rdfds, wrfds;

	printf("Session poll...\n");
	memcpy(&rdfds, &mrdfds, sizeof(fd_set));
	memcpy(&wrfds, &mwrfds, sizeof(fd_set));
	if ((n = select(maxfds, &rdfds, &wrfds, NULL, NULL)) < 0) {
		perror("session_poll: select");
		return(-1);
	}
	printf("select returned %d\n", n);
}

/*
 * Take a free session from the freelist or allocate a new one.
 */
struct session *
session_alloc()
{
	struct session *sp;

	if ((sp = sfreelist) != NULL)
		sfreelist = sp->next;
	else if ((sp = (struct session *)malloc(sizeof(struct session))) == NULL) {
		perror("session_alloc: malloc failed");
		exit(1);
	}
	memset((char *)sp, 0, sizeof(struct session));
	return(sp);
}

/*
 * Free up a session struct.
 */
void
session_free(struct session *sp)
{
	sp->next = sfreelist;
	sfreelist = sp;
}

/*
 *
 */
static void
add_fd(int fd, int which)
{
	if (which & FD_READ)
		FD_SET(fd, &mrdfds);
	if (which & FD_WRITE)
		FD_SET(fd, &mwrfds);
	if (fd >= maxfds)
		maxfds = fd + 1;
}
