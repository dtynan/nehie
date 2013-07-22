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

#define BUFFER_SIZE	4096

/*
 * Individual buffer used for data storage.
 */
struct	buffer	{
	struct	buffer	*next;
	int		size;
	unsigned char	*data;
};

/*
 * Queue of buffers.
 */
struct	bqueue	{
	struct	bqueue	*next;
	int		size;
	struct	buffer	*head;
	struct	buffer	*tail;
};

/*
 * A live session from a client.
 */
struct	session	{
	struct	session	*next;
	int		type;
	int		state;
	int		client_fd;
	struct	bqueue	*rdqueue;
	struct	bqueue	*wrqueue;
	void		(*rdfunc)();
	void		(*wrfunc)();
};

/* Values for session.type */
#define SESSION_MASTER	0
#define SESSION_CLIENT	1

/* Values for session.state */
#define SESSION_DEAD	0
#define SESSION_OPEN	1

/*
 * Prototypes...
 */
void		session_init();
struct session	*session_listen(char *uri);
int		session_poll(struct session *);
struct session	*session_alloc();
void		session_free(struct session *);
