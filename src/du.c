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
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <strings.h>
#include <string.h>
#define EQ(x,y)	(strcmp(x,y)==0)
#define ML	1000
#define BSIZE 512
#define BSHIFT 9
#define DIRSIZ NAME_MAX

struct stat Statb;
char	path[256], name[256];
int	Aflag = 0,
	Sflag = 0,
	Noarg = 0;
struct {
	int	dev,
		ino;
} ml[ML];
long	descend(char *np, char *fname);

main(argc, argv)
char **argv;
{
	register	i = 1;
	long	blocks = 0;
	register char	*np;

	if (argc>1) {
		if(EQ(argv[i], "-s")) {
			++i;
			++Sflag;
		} else if(EQ(argv[i], "-a")) {
			++i;
			++Aflag;
		}
	}
	if(i == argc)
		++Noarg;

	do {
		strcpy(path, Noarg? ".": argv[i]);
		strcpy(name, path);
		if(np = rindex(name, '/')) {
			*np++ = '\0';
			if(chdir(*name? name: "/") == -1) {
				fprintf(stderr, "cannot chdir()\n");
				exit(1);
			}
		} else
			np = path;
		blocks = descend(path, *np? np: ".");
		if(Sflag)
			printf("%ld	%s\n", blocks, path);
	} while(++i < argc);

	exit(0);
}

long
descend(np, fname)
char *np, *fname;
{
	DIR* dir = NULL; /* open directory */

	struct dirent *entry;
	char *c1, *c2;
	int i;
	char *endofname;
	long blocks = 0;

	if(stat(fname,&Statb)<0) {
		fprintf(stderr, "--bad status < %s >\n", name);
		return 0L;
	}
	if(Statb.st_nlink > 1 && !(S_ISDIR(Statb.st_mode))) {
		static linked = 0;

		for(i = 0; i <= linked; ++i) {
			if(ml[i].ino==Statb.st_ino && ml[i].dev==Statb.st_dev)
				return 0;
		}
		if (linked < ML) {
			ml[linked].dev = Statb.st_dev;
			ml[linked].ino = Statb.st_ino;
			++linked;
		}
	}
	blocks = (Statb.st_size + BSIZE-1) >> BSHIFT;

	if(!(S_ISDIR(Statb.st_mode))) {
		if(Aflag)
			printf("%ld	%s\n", blocks, np);
		return(blocks);
	}

	for(c1 = np; *c1; ++c1);
	if(*(c1-1) == '/')
		--c1;
	endofname = c1;
	if(chdir(fname) == -1)
		return 0;
	for(;;){ /* each block */
		if(!dir) {
			if((dir=opendir("."))==NULL) {
				fprintf(stderr, "--cannot open < %s >\n",
					np);
				goto ret;
			}
		}
		if((entry=readdir(dir))==NULL){
			break;
		}
		{
			/* each directory entry */
			if(entry->d_ino==0
			|| EQ(entry->d_name, ".")
			|| EQ(entry->d_name, ".."))
				continue;
			c1 = endofname;
			*c1++ = '/';
			c2 = entry->d_name;
			for(i=0; i<DIRSIZ; ++i)
				if(*c2)
					*c1++ = *c2++;
				else
					break;
			*c1 = '\0';
			if(c1 == endofname) /* ?? */
				return 0L;
			blocks += descend(np, endofname+1);
		}
	}
	*endofname = '\0';
	if(!Sflag)
		printf("%ld	%s\n", blocks, np);
ret:
	if(dir)
		closedir(dir);
	if(chdir("..") == -1) {
		*endofname = '\0';
		fprintf(stderr, "Bad directory <%s>\n", np);
		while(*--endofname != '/');
		*endofname = '\0';
		if(chdir(np) == -1)
			exit(1);
	}
	return(blocks);
}
