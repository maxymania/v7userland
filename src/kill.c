/*
 * Copyright (c) 2016 Simon Schmidt
 * 
 * Copyright(C) Caldera International Inc.  2001-2002.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * - Redistributions of source code and documentation must retain the above
 *   copyright notice, this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * - All advertising materials mentioning features or use of this software must
 *   display the following acknowledgement: This product includes software developed
 *   or owned by  Caldera International, Inc.
 * 
 * - Neither the name of Caldera International, Inc. nor the names of other
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 * 
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA INTERNATIONAL, INC.
 * AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE LIABLE FOR
 * ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * kill - send signal to process
 */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	register int signo, pid, res;
	int errlev;
	//extern char *sys_errlist[];
	//extern errno;

	errlev = 0;
	if (argc <= 1) {
	usage:
		printf("usage: kill [ -signo ] pid ...\n");
		exit(2);
	}
	if (*argv[1] == '-') {
		signo = atoi(argv[1]+1);
		argc--;
		argv++;
	} else
		signo = SIGTERM;
	argv++;
	while (argc > 1) {
		if (**argv<'0' || **argv>'9')
			goto usage;
		res = kill(pid = atoi(*argv), signo);
		if (res<0) {
			printf("%u: %s\n", pid, strerror(errno));
			errlev = 1;
		}
		argc--;
		argv++;
	}
	return(errlev);
}
