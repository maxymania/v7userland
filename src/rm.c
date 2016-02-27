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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <dirent.h>

int	errcode;

int yes();
int rmdir_rm(char *f, int iflg);
int dotname(char *s);
void rm(char arg[], int fflg, int rflg, int iflg, int level);

void main(int argc, char *argv[])
{
	register char *arg;
	int fflg, iflg, rflg;

	fflg = 0;
	if (isatty(0) == 0)
		fflg++;
	iflg = 0;
	rflg = 0;
	if(argc>1 && argv[1][0]=='-') {
		arg = *++argv;
		argc--;
		while(*++arg != '\0')
			switch(*arg) {
			case 'f':
				fflg++;
				break;
			case 'i':
				iflg++;
				break;
			case 'r':
				rflg++;
				break;
			default:
				printf("rm: unknown option %s\n", *argv);
				exit(1);
			}
	}
	while(--argc > 0) {
		if(!strcmp(*++argv, "..")) {
			fprintf(stderr, "rm: cannot remove `..'\n");
			continue;
		}
		rm(*argv, fflg, rflg, iflg, 0);
	}

	exit(errcode);
}

void rm(char arg[], int fflg, int rflg, int iflg, int level)
{
	struct stat buf;
	struct dirent *dirent;
	char name[100];
	//int d;
	DIR* d;

	if(stat(arg, &buf)) {
		if (fflg==0) {
			printf("rm: %s nonexistent\n", arg);
			++errcode;
		}
		return;
	}
	if ((buf.st_mode&S_IFMT) == S_IFDIR) {
		if(rflg) {
			if (access(arg, 02) < 0) {
				if (fflg==0)
					printf("%s not changed\n", arg);
				errcode++;
				return;
			}
			if(iflg && level!=0) {
				printf("directory %s: ", arg);
				if(!yes())
					return;
			}
			if((d=opendir(arg)) < 0) {
				printf("rm: %s: cannot read\n", arg);
				exit(1);
			}
			while((dirent = readdir(d))!=NULL) {
				if(dirent->d_ino != 0 && !dotname(dirent->d_name)) {
					sprintf(name, "%s/%.14s", arg, dirent->d_name);
					rm(name, fflg, rflg, iflg, level+1);
				}
			}
			closedir(d);
			errcode += rmdir_rm(arg, iflg);
			return;
		}
		printf("rm: %s directory\n", arg);
		++errcode;
		return;
	}

	if(iflg) {
		printf("%s: ", arg);
		if(!yes())
			return;
	}
	else if(!fflg) {
		if (access(arg, W_OK)<0) {
			printf("rm: %s %o mode ", arg, buf.st_mode&0777);
			if(!yes())
				return;
		}
	}
	if(unlink(arg) && (fflg==0 || iflg)) {
		printf("rm: %s not removed\n", arg);
		++errcode;
	}
}

int dotname(char *s)
{
	if(s[0] == '.')
		if(s[1] == '.')
			if(s[2] == '\0')
				return(1);
			else
				return(0);
		else if(s[1] == '\0')
			return(1);
	return(0);
}

int rmdir_rm(char *f, int iflg)
{
	int status, i;

	if(dotname(f))
		return(0);
	if(iflg) {
		printf("%s: ", f);
		if(!yes())
			return(0);
	}
	;
	return rmdir(f)?1:0;
}

int yes()
{
	int i, b;

	i = b = getchar();
	while(b != '\n' && b != EOF)
		b = getchar();
	return(i == 'y');
}
