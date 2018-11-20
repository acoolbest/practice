#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

char gettype(mode_t mod)
{
    /*
       if( S_ISREG(mod) ) return '-';
       if( S_ISDIR(mod) ) return 'd';
       if( S_ISCHR(mod) ) return 'c';
       if( S_ISBLK(mod) ) return 'b';
       if( S_ISLNK(mod) ) return 'l';
       if( S_ISSOCK(mod) ) return 's';
       if( S_ISFIFO(mod) ) return 'p';
       */
    switch(mod & S_IFMT) {
        case S_IFSOCK:
            return 's';
        case S_IFREG:
            return '-';
        case S_IFCHR:
            return 'c';
        case S_IFBLK:
            return 'b';
        case S_IFLNK:
            return 'l';
        case S_IFIFO:
            return 'p';
        case S_IFDIR:
            return 'd';
    }
}

char *getperm(mode_t mod)
{
    static char buf[10];

    int i = 9;
    while(i--) {
        if(mod & 1<<i) {
            switch((8-i)%3) {
                case 0:
                    buf[8-i] = 'r';
                    break;// r/w/x;
                case 1:
                    buf[8-i] = 'w';
                    break;// r/w/x;
                case 2:
                    buf[8-i] = 'x';
                    break;// r/w/x;
            }
        } else
            buf[8-i] = '-';
    }

    return buf;
}

char *getttt(time_t *t)
{
    static char buf[50];

    strncpy(buf, ctime(t), 24);

    return buf;
}

int main(int argc, char **argv)
{
    DIR *dp = opendir((2==argc)?argv[1]:".");

    if(2 == argc)
        chdir(argv[1]);

    struct dirent *p;

    struct stat s;
    while( p = readdir(dp) ) {
        if(-1 == stat(p->d_name, &s) ) {
            perror("stat");
            return -1;
        }

        printf( "%c%s %5d %s %s %10ld %s\033[1;31;40m %s  \033[0m\n",
                gettype(s.st_mode),
                getperm(s.st_mode),
                s.st_nlink,
                getpwuid(s.st_uid)->pw_name,
                getgrgid(s.st_gid)->gr_name,
                s.st_size,
                getttt(&s.st_mtime),
                p->d_name );
    }
}
