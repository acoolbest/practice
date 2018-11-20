/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */

int main ()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer,80,"Now it's %I:%M%p %z %Z.",timeinfo);
	puts (buffer);
	printf("%s\n", asctime(timeinfo));

	timeinfo = gmtime(&rawtime);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GTM.",timeinfo);
	puts (buffer);
	
	time_t rawtime1 = time(NULL);
	time_t rawtime2 = time(NULL) + 8*3600;
	timeinfo = gmtime(&rawtime1);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GTM.",timeinfo);
	puts (buffer);
	timeinfo = gmtime(&rawtime2);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S CST.",timeinfo);
	puts (buffer);
	
	


	return 0;
}
