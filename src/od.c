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
 * od -- octal (also hex, decimal, and character) dump
 */

#include <stdio.h>
#include <stdlib.h>

unsigned short	word[8];
unsigned short	lastword[8];
int	conv;
int	base =	010;
int	max;
long	addr;

void line(long a, unsigned short *w, int n);
void putx(unsigned int n,int c);
void cput(int c);
void putn(long n, int b, int c);
void pre(int n);
void offset(char* s);

void main(int argc, char **argv)
{
	char *p;
	int n, f, same;


	argv++;
	f = 0;
	if(argc > 1) {
		p = *argv;
		if(*p == '-') {
			while(*p != '\0') {
				switch(*p++) {
				case 'o':
					conv |= 001;
					f = 6;
					break;
				case 'd':
					conv |= 002;
					f = 5;
					break;
				case 'x':
				case 'h':
					conv |= 010;
					f = 4;
					break;
				case 'c':
					conv |= 020;
					f = 7;
					break;
				case 'b':
					conv |= 040;
					f = 7;
					break;
				}
				if(f > max)
					max = f;
			}
			argc--;
			argv++;
		}
	}
	if(!conv) {
		max = 6;
		conv = 1;
	}
	if(argc > 1)
	if(**argv != '+') {
		if (freopen(*argv, "r", stdin) == NULL) {
			printf("cannot open %s\n", *argv);
			exit(1);
		}
		argv++;
		argc--;
	}
	if(argc > 1)
		offset(*argv);

	same = -1;
	for ( ; (n = fread((char *)word, 1, sizeof(word), stdin)) > 0; addr += n) {
		if (same>=0) {
			for (f=0; f<8; f++)
				if (lastword[f] != word[f])
					goto notsame;
			if (same==0) {
				printf("*\n");
				same = 1;
			}
			continue;
		}
	notsame:
		line(addr, word, (n+sizeof(word[0])-1)/sizeof(word[0]));
		same = 0;
		for (f=0; f<8; f++)
			lastword[f] = word[f];
		for (f=0; f<8; f++)
			word[f] = 0;
	}
	putn(addr, base, 7);
	putchar('\n');
}

void line(long a, unsigned short *w, int n)
{
	int i, f, c;

	f = 1;
	for(c=1; c; c<<=1) {
		if((c&conv) == 0)
			continue;
		if(f) {
			putn(a, base, 7);
			putchar(' ');
			f = 0;
		} else
			putchar('\t');
		for (i=0; i<n; i++) {
			putx(w[i], c);
			putchar(i==n-1? '\n': ' ');
		}
	}
}

void putx(unsigned int n,int c)
{

	switch(c) {
	case 001:
		pre(6);
		putn((long)n, 8, 6);
		break;
	case 002:
		pre(5);
		putn((long)n, 10, 5);
		break;
	case 010:
		pre(4);
		putn((long)n, 16, 4);
		break;
	case 020:
		pre(7);
		{
			unsigned short sn = n;
			cput(*(char *)&sn);
			putchar(' ');
			cput(*((char *)&sn + 1));
			break;
		}
	case 040:
		pre(7);
		{
			unsigned short sn = n;
			putn((long)(*(char *)&sn)&0377, 8, 3);
			putchar(' ');
			putn((long)(*((char *)&sn + 1))&0377, 8, 3);
			break;
		}
	}
}

void cput(int c)
{
	c &= 0377;
	if(c>037 && c<0177) {
		printf("  ");
		putchar(c);
		return;
	}
	switch(c) {
	case '\0':
		printf(" \\0");
		break;
	case '\b':
		printf(" \\b");
		break;
	case '\f':
		printf(" \\f");
		break;
	case '\n':
		printf(" \\n");
		break;
	case '\r':
		printf(" \\r");
		break;
	case '\t':
		printf(" \\t");
		break;
	default:
		putn((long)c, 8, 3);
	}
}

void putn(long n, int b, int c)
{
	int d;

	if(!c)
		return;
	putn(n/b, b, c-1);
	d = n%b;
	if (d > 9)
		putchar(d-10+'a');
	else
		putchar(d+'0');
}

void pre(int n)
{
	int i;

	for(i=n; i<max; i++)
		putchar(' ');
}

void offset(char* s)
{
	register char *p;
	long a;
	register int d;

	if (*s=='+')
		s++;
	if (*s=='x') {
		s++;
		base = 16;
	} else if (*s=='0' && s[1]=='x') {
		s += 2;
		base = 16;
	} else if (*s == '0')
		base = 8;
	p = s;
	while(*p) {
		if (*p++=='.')
			base = 10;
	}
	for (a=0; *s; s++) {
		d = *s;
		if(d>='0' && d<='9')
			a = a*base + d - '0';
		else if (d>='a' && d<='f' && base==16)
			a = a*base + d + 10 - 'a';
		else
			break;
	}
	if (*s == '.')
		s++;
	if(*s=='b' || *s=='B')
		a *= 512;
	fseek(stdin, a, 0);
	addr = a;
}


