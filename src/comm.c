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
#define LB 256
int	one;
int	two;
int	three;

char	*ldr[3];

int rd(FILE *file,char *buf);
void wr(char* str,int n);
void copy(FILE* ibuf,char* lbuf,int n);
int compare(char* a,char* b);
FILE *openfil(char *s);

FILE *ib1;
FILE *ib2;

int main(int argc,char *argv[])
{
	int	l;
	char	lb1[LB],lb2[LB];

	ldr[0] = "";
	ldr[1] = "\t";
	ldr[2] = "\t\t";
	if(argc > 1)  {
		if(*argv[1] == '-' && argv[1][1] != 0) {
			l = 1;
			while(*++argv[1]) {
				switch(*argv[1]) {
				case'1':
					if(!one) {
						one = 1;
						ldr[1][0] =
						ldr[2][l--] = '\0';
					}
					break;
				case '2':
					if(!two) {
						two = 1;
						ldr[2][l--] = '\0';
					}
					break;
				case '3':
					three = 1;
					break;
				default:
				fprintf(stderr,"comm: illegal flag\n");
				exit(1);
				}
			}
			argv++;
			argc--;
		}
	}

	if(argc < 3) {
		fprintf(stderr,"comm: arg count\n");
		exit(1);
	}

	ib1 = openfil(argv[1]);
	ib2 = openfil(argv[2]);


	if(rd(ib1,lb1) < 0) {
		if(rd(ib2,lb2) < 0)	exit(0);
		copy(ib2,lb2,2);
	}
	if(rd(ib2,lb2) < 0)	copy(ib1,lb1,1);

	while(1) {

		switch(compare(lb1,lb2)) {

			case 0:
				wr(lb1,3);
				if(rd(ib1,lb1) < 0) {
					if(rd(ib2,lb2) < 0)	exit(0);
					copy(ib2,lb2,2);
				}
				if(rd(ib2,lb2) < 0)	copy(ib1,lb1,1);
				continue;

			case 1:
				wr(lb1,1);
				if(rd(ib1,lb1) < 0)	copy(ib2,lb2,2);
				continue;

			case 2:
				wr(lb2,2);
				if(rd(ib2,lb2) < 0)	copy(ib1,lb1,1);
				continue;
		}
	}
}

int rd(FILE *file,char *buf)
{

	register int i, c;
	i = 0;
	while((c = getc(file)) != EOF) {
		*buf = c;
		if(c == '\n' || i > LB-2) {
			*buf = '\0';
			return(0);
		}
		i++;
		buf++;
	}
	return(-1);
}

void wr(char* str,int n)
{

	switch(n) {

		case 1:
			if(one)	return;
			break;

		case 2:
			if(two)	return;
			break;

		case 3:
			if(three)	return;
	}
	printf("%s%s\n",ldr[n-1],str);
}

void copy(FILE* ibuf,char* lbuf,int n)
{
	do {
		wr(lbuf,n);
	} while(rd(ibuf,lbuf) >= 0);

	exit(0);
}

int compare(char* a,char* b)
{
	register char *ra,*rb;

	ra = --a;
	rb = --b;
	while(*++ra == *++rb)
		if(*ra == '\0')	return(0);
	if(*ra < *rb)	return(1);
	return(2);
}
FILE *openfil(char *s)
{
	FILE *b;
	if(s[0]=='-' && s[1]==0)
		b = stdin;
	else if((b=fopen(s,"r")) == NULL) {
		fprintf(stderr,"comm: cannot open %s\n",s);
		exit(1);
	}
	return(b);
}
