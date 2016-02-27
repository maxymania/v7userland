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
 * mv file1 file2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <signal.h>
#include <unistd.h>

#define	DOT	"."
#define	DOTDOT	".."
#define	DELIM	'/'
#define SDELIM "/"
#define	MAXN	100
#define MODEBITS 07777
#define ROOTINO 2

//char	*pname();
//char	*sprintf();
//char	*dname();
struct	stat s1, s2;

static
char* buffer;

static
unsigned int len,foldlen;

static
const char* buf_jnr;

static
const char* fcct(const char* folder,const char* file){
	unsigned int filelen = strlen(file);
	if(!buffer){
		foldlen = strlen(folder);
		if(folder[foldlen-1]=='/')
			buf_jnr = "%s%s";
		else
			buf_jnr = "%s/%s";
		len = foldlen+filelen;
		buffer = malloc(len+2);
		if(!buffer)exit(1);
	}else if(len<foldlen+filelen){
		len = foldlen+filelen;
		buffer = realloc(buffer,len+2);
		if(!buffer)exit(1);
	}
	sprintf(buffer,buf_jnr,folder,file);
	return buffer;
}

int
main(int argc, char *argv[])
{
	buffer = NULL;
	len = 0;
	int i, r;

	if (argc < 3)
		goto usage;
	if (stat(argv[1], &s1) < 0) {
		fprintf(stderr, "mv: cannot access %s\n", argv[1]);
		return(1);
	}
	if ((s1.st_mode & S_IFMT) == S_IFDIR) {
		if (argc != 3)
			goto usage;
		return rename(argv[1], argv[2])?1:0;
	}
	setuid(getuid());
	if (argc > 3)
		if (stat(argv[argc-1], &s2) < 0 || (s2.st_mode & S_IFMT) != S_IFDIR)
			goto usage;
	r = 0;
	
	if (argc == 3)
		r |= rename(argv[1],argv[2]);
	else for (i=1; i<argc-1; i++)
		r |= rename(argv[i], fcct(argv[argc-1],argv[i]));
	return(r?1:0);
usage:
	fprintf(stderr, "usage: mv f1 f2; or mv d1 d2; or mv f1 ... fn d1\n");
	return(1);
}
