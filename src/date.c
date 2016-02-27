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
 * date : print date
 * date YYMMDDHHMM[.SS] : set date, if allowed
 * date -u ... : date in GMT
 */
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <utmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

long	timbuf;
char	*ap, *ep, *sp;
int	uflag;

//char	*timezone();
static	int	dmsize[12] =
{
	31,
	28,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};

struct utmp wtmp[2] = { {.ut_line="|", .ut_name="", .ut_time=0}, {.ut_line="{", .ut_name="", .ut_time=0}};

time_t timevalue;
char	*ctime();
char	*asctime();
struct	tm *localtime();
struct	tm *gmtime();

#define timezone(...) tzname[0]
int gtime();
int gp(int dfault);

int main(int argc, char *argv[])
{
	register char *tzn;
	struct timeb info;
	int wf, rc;

	rc = 0;
	ftime(&info);
	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='u') {
		argc--;
		argv++;
		uflag++;
	}
	if(argc > 1) {
		ap = argv[1];
		if (gtime()) {
			printf("date: bad conversion\n");
			exit(1);
		}
		/* convert to GMT assuming local time */
		if (uflag==0) {
			timbuf += (long)info.timezone*60;
			/* now fix up local daylight time */
			if(localtime(&timbuf)->tm_isdst)
				timbuf -= 60*60;
		}
		time(&timevalue);
		wtmp[0].ut_time = timevalue;
		if(stime(&timbuf) < 0) {
			rc++;
			printf("date: no permission\n");
			
			// TODO: check!
		} else if ((wf = open("/usr/adm/wtmp", O_WRONLY|O_CREAT|O_APPEND)) >= 0) {
			wtmp[1].ut_time = timevalue;
			//lseek(wf, 0L, 2);
			write(wf, (char *)wtmp, sizeof(wtmp));
			close(wf);
		}
	}
	if (rc==0)
		time(&timbuf);
	if(uflag) {
		ap = asctime(gmtime(&timbuf));
		tzn = "GMT";
	} else {
		struct tm *tp;
		tp = localtime(&timbuf);
		ap = asctime(tp);
		tzn = timezone(info.timezone, tp->tm_isdst);
	}
	printf("%.20s", ap);
	if (tzn)
		printf("%s", tzn);
	printf("%s", ap+19);
	exit(rc);
}

int gtime()
{
	register int i, year, month;
	int day, hour, mins, secs;
	struct tm *L;
	char x;

	ep=ap;
	while(*ep) ep++;
	sp=ap;
	while(sp<ep) {
		x = *sp;
		*sp++ = *--ep;
		*ep = x;
	}
	sp=ap;
	time(&timbuf);
	L=localtime(&timbuf);
	secs = gp(-1);
	if(*sp!='.') {
		mins=secs;
		secs=0;
	} else {sp++;
		mins = gp(-1);
	}
	hour = gp(-1);
	day = gp(L->tm_mday);
	month = gp(L->tm_mon+1);
	year = gp(L->tm_year);
	if(*sp)
		return(1);
	if( month<1 || month>12 ||
	    day<1 || day>31 ||
	    mins<0 || mins>59 ||
	    secs<0 || secs>59)
		return(1);
	if (hour==24) {
		hour=0; day++;
	}
	if (hour<0 || hour>23)
		return(1);
	timbuf = 0;
	year += 1900;
	for(i=1970; i<year; i++)
		timbuf += dysize(i);
	/* Leap year */
	if (dysize(year)==366 && month >= 3)
		timbuf++;
	while(--month)
		timbuf += dmsize[month-1];
	timbuf += day-1;
	timbuf = 24*timbuf + hour;
	timbuf = 60*timbuf + mins;
	timbuf = 60*timbuf + secs;
	return(0);

}

int gp(int dfault)
{
	register int c, d;

	if(*sp==0)
		return(dfault);
	c = (*sp++)-'0';
	d = (*sp ? (*sp++)-'0' : 0);
	if(c<0 || c>9 || d<0 || d>9)
		return(-1);
	return(c+10*d);
}

