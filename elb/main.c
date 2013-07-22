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
#include <string.h>

#include "nehie.h"
#include "elb.h"

int	verbose;

void	usage();

/*
 * All load balancer life begins here...
 */
int
main(int argc, char *argv[])
{
	int i;
	struct session *sp;

	while ((i = getopt(argc, argv, "v")) != EOF) {
		switch (i) {
		case 'v':
			verbose = 1;
			break;

		default:
			usage();
		}
	}
	printf("Do init work...\n");
	session_init();
	printf("Do listen work...\n");
	if ((sp = session_listen("0.0.0.0:7000")) == NULL) {
		fprintf(stderr, "elbd: cannot create listen session.\n");
		exit(1);
	}
	while (session_poll(sp) >= 0)
		;
	exit(0);
}

/*
 * 
 */
void
usage()
{
	fprintf(stderr, "Usage: elbd [-v]\n");
	exit(2);
}
