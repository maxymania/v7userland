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
 * cp oldfile newfile
 */

#define	BSIZE	512
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
struct	stat	stbuf1, stbuf2;
char	iobuf[BSIZE];

int copy(char *from,char *to);

void main(int argc, char *argv[])
{
	int i, r;

	if (argc < 3) 
		goto usage;
	if (argc > 3) {
		if (stat(argv[argc-1], &stbuf2) < 0)
			goto usage;
		if ((stbuf2.st_mode&S_IFMT) != S_IFDIR) 
			goto usage;
	}
	r = 0;
	for(i=1; i<argc-1;i++)
		r |= copy(argv[i], argv[argc-1]);
	exit(r);
usage:
	fprintf(stderr, "Usage: cp: f1 f2; or cp f1 ... fn d2\n");
	exit(1);
}

int copy(char *from,char *to)
{
	int fold, fnew, n;
	register char *p1, *p2, *bp;
	int mode;
	if ((fold = open(from, O_RDONLY)) < 0) {
		fprintf(stderr, "cp: cannot open %s\n", from);
		return(1);
	}
	fstat(fold, &stbuf1);
	mode = stbuf1.st_mode;
	/* is target a directory? */
	if (stat(to, &stbuf2) >=0 &&
	   (stbuf2.st_mode&S_IFMT) == S_IFDIR) {
		p1 = from;
		p2 = to;
		bp = iobuf;
		while(*bp++ = *p2++)
			;
		bp[-1] = '/';
		p2 = bp;
		while(*bp = *p1++)
			if (*bp++ == '/')
				bp = p2;
		to = iobuf;
	}
	if (stat(to, &stbuf2) >= 0) {
		if (stbuf1.st_dev == stbuf2.st_dev &&
		   stbuf1.st_ino == stbuf2.st_ino) {
			fprintf(stderr, "cp: cannot copy file to itself.\n");
			return(1);
		}
	}
	if ((fnew = open(to, O_WRONLY|O_CREAT|O_TRUNC, mode)) < 0) {
		fprintf(stderr, "cp: cannot create %s\n", to);
		close(fold);
		return(1);
	}
	while(n = read(fold,  iobuf,  BSIZE)) {
		if (n < 0) {
			fprintf(stderr, "cp: read error\n");
			close(fold);
			close(fnew);
			return(1);
		} else
			if (write(fnew, iobuf, n) != n) {
				fprintf(stderr, "cp: write error.\n");
				close(fold);
				close(fnew);
				return(1);
			}
	}
	close(fold);
	close(fnew);
	return(0);
}
