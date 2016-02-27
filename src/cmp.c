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
#include <ctype.h>

FILE	*file1,*file2;
int	eflg;
int	lflg	= 1;
long	line	= 1;
long	chr	= 0;
long	skip1;
long	skip2;

long	otoi(char *s);

int main(int argc, char **argv)
{
	int c1, c2;
	char *arg;

	if(argc < 3)
		goto narg;
	arg = argv[1];
	if(arg[0] == '-' && arg[1] == 's') {
		lflg--;
		argv++;
		argc--;
	}
	arg = argv[1];
	if(arg[0] == '-' && arg[1] == 'l') {
		lflg++;
		argv++;
		argc--;
	}
	if(argc < 3)
		goto narg;
	arg = argv[1];
	if( arg[0]=='-' && arg[1]==0 )
		file1 = stdin;
	else if((file1 = fopen(arg, "r")) == NULL)
		goto barg;
	arg = argv[2];
	if((file2 = fopen(arg, "r")) == NULL)
		goto barg;
	if (argc>3)
		skip1 = otoi(argv[3]);
	if (argc>4)
		skip2 = otoi(argv[4]);
	while (skip1) {
		if ((c1 = getc(file1)) == EOF) {
			arg = argv[1];
			goto earg;
		}
		skip1--;
	}
	while (skip2) {
		if ((c2 = getc(file2)) == EOF) {
			arg = argv[2];
			goto earg;
		}
		skip2--;
	}

loop:
	chr++;
	c1 = getc(file1);
	c2 = getc(file2);
	if(c1 == c2) {
		if (c1 == '\n')
			line++;
		if(c1 == EOF) {
			if(eflg)
				exit(1);
			exit(0);
		}
		goto loop;
	}
	if(lflg == 0)
		exit(1);
	if(c1 == EOF) {
		arg = argv[1];
		goto earg;
	}
	if(c2 == EOF)
		goto earg;
	if(lflg == 1) {
		printf("%s %s differ: char %ld, line %ld\n", argv[1], arg,
			chr, line);
		exit(1);
	}
	eflg = 1;
	printf("%6ld %3o %3o\n", chr, c1, c2);
	goto loop;

narg:
	printf("cmp: arg count\n");
	exit(2);

barg:
	if (lflg)
	printf("cmp: cannot open %s\n", arg);
	exit(2);

earg:
	printf("cmp: EOF on %s\n", arg);
	exit(1);
}

long otoi(char *s)
{
	long v;
	int base;

	v = 0;
	base = 10;
	if (*s == '0')
		base = 8;
	while(isdigit(*s))
		v = v*base + *s++ - '0';
	return(v);
}
