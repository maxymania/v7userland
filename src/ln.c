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
 * ln [ -f ] target [ new name ]
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char	*rindex();

int main(int argc,char **argv)
{
	struct stat statb;
	register char *np;
	int fflag = 0;
	char nb[100], *name=nb, *arg2;
	int statres;

	if (argc >1 && strcmp(argv[1], "-f")==0) {
		argc--;
		argv++;
		fflag++;
	}
	if (argc<2 || argc>3) {
		printf("Usage: ln target [ newname ]\n");
		exit(1);
	}
	np = rindex(argv[1], '/');
	if (np==0)
		np = argv[1];
	else
		np++;
	if (argc==2)
		arg2 = np;
	else
		arg2 = argv[2];
	statres = stat(argv[1], &statb);
	if (statres<0) {
		printf ("ln: %s does not exist\n", argv[1]);
		exit(1);
	}
	if (fflag==0 && (statb.st_mode&S_IFMT) == S_IFDIR) {
		printf("ln: %s is a directory\n", argv[1]);
		exit(1);
	}
	statres = stat(arg2, &statb);
	if (statres>=0 && (statb.st_mode&S_IFMT) == S_IFDIR)
		sprintf(name, "%s/%s", arg2, np);
	else
		name = arg2;
	if (link(argv[1], name)<0) {
		perror("ln");
		exit(1);
	}
	exit(0);
}
