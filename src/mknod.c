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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int number(char *s);

void main(int argc, char **argv)
{
	int m, a, b;

	if(argc != 5) {
		printf("arg count\n");
		goto usage;
	}
	if(*argv[2] == 'b')
		m = 060666; else
	if(*argv[2] == 'c')
		m = 020666; else
		goto usage;
	a = number(argv[3]);
	if(a < 0)
		goto usage;
	b = number(argv[4]);
	if(b < 0)
		goto usage;
	if(mknod(argv[1], m, (a<<8)|b) < 0)
		perror("mknod");
	exit(0);

usage:
	printf("usage: mknod name b/c major minor\n");
}

int number(char *s)
{
	int n, c;

	n = 0;
	while(c = *s++) {
		if(c<'0' || c>'9')
			return(-1);
		n = n*10 + c-'0';
	}
	return(n);
}
