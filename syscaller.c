/*
 * syscaller v0.9 - breaking out of chroot jails "ex nihilo"
 *
 * by Derek Callaway <decal@security-objectives.com>
 *
 *
 * Executes system calls instead of relying on programs from the
 * GNU/Linux binutils package. Can be useful for breaking out of
 * a chroot() jail or restricted shell situation.
 *
 * compile: gcc -O2 -o syscaller -c syscaller.c -Wall -ansi -pedantic
 * copy: cat syscaller | ssh -l user@host.dom 'cat>syscaller'
 *
 * If the cat binary isn't present in the jail, you'll have to be more
 * creative and use a shell builtin like echo (i.e. not the echo binary,
 * but bash's internal implementation of it.)
 *
 * Without any locally accessible file download programs such as:
 * scp, tftp, netcat, sftp, wget, curl, rz/sz, kermit, lynx, etc.
 * You'll have to create the binary on the target system manually.
 * i.e. by echo'ing hexadecimal bytecode. This is left as an exercise
 * to the reader.
 *
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>
#include<fcntl.h>

#define _GNU_SOURCE 1
#define _USE_MISC 1

#include<unistd.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>

int syscall(int number, ...);

/* This is for chdir() */
#define SHELL_PATHNAME "/bin/sh"

static void usage(char **argv)
{
  printf("usage: %s syscall arg1 [arg2 [...]]\n", *argv);
  printf("help:  %s help\n", *argv);
  exit(EXIT_FAILURE);
}

static void help(char **argv)
{
  puts("syscaller v0.9");
  puts("=-=-=-=-=-=-=-=");
  puts("");
  puts("SYSCALLER COMMANDS");
  puts("");
  puts("chmod mode pathname");
  puts("chdir pathname");
  puts("chown user group pathname");
  puts("mkdir pathname mode");
  puts("rmdir pathname");
  puts("touch pathname mode");
  puts("");

  puts("Note: modes are in octal format (symbolic modes are unsupported)");
  puts("Note: some commands mask octal mode bits with the current umask val                 ue");
  puts("Note: creat is an alias for touch");
  puts("");
  puts("USEFUL SHELL BUILTINS");
  puts("");
  puts("ls -a / (via brace/pathname expansion): echo /{.*,*}");

  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  register char *p = 0;
  signed auto int r = 1;

  if(argc < 2)
    usage(argv);

  /* I prefer to avoid strcasecmp() since it's not really standard C. */
  for(p = argv[1];*p;++p)
    *p = tolower(*p);

  do
  {
    if(!strcmp(argv[1], "chmod") && argc >= 4)
    {
      /* decimal to octal integer conversion */
      const mode_t m = strtol(argv[2], NULL, 8);

      r = syscall(SYS_chmod, argv[3], m);

#ifdef DEBUG
  fprintf(stderr, "syscall(%d, %s, %d) => %d\n", SYS_chmod, argv[3], m, r);
#endif
    }
    else if((!strcmp(argv[1], "chdir") || !strcmp(argv[1], "cd")) && argc >                 = 3)
    {
      static char *const av[] = {SHELL_PATHNAME, NULL};
      auto signed int r2 = 0;

      r = syscall(SYS_chdir, argv[2]);

#ifdef DEBUG
  fprintf(stderr, "syscall(%d, %s) => %d\n", SYS_chdir, argv[2], r);
#endif

      /* This is required because the new current working directory isn't
       * bound to the original login shell. */
      printf("[%s] exec'ing new shell in directory: %s\n", *argv, argv[2]);
      r2 = system(av[0]);
      printf("[%s] leaving shell in child process\n", *argv);

      if(r2 < 0)
        r = r2;
    }
    else if(!strcmp(argv[1], "chown") && argc >= 5)
    {
      struct passwd *u = NULL;
      struct group *g = NULL;

      if(!(u = getpwnam(argv[2])))
        break;

#ifdef DEBUG
  fprintf(stderr, "getpwnam(%s) => %s:%s:%d:%d:%s:%s:%s\n", argv[2], u->pw_                 name, u->pw_passwd, u->pw_uid, u->pw_gid, u->pw_gecos, u->pw_dir, u->pw_she                 ll);
#endif

      if(!(g = getgrnam(argv[3])))

        break;

#ifdef DEBUG
  fprintf(stderr, "getgrnam(%s) => %s:%s:%s:%s:", argv[3], g->gr_nam, g->gr                 _passwd, g->gr_gid);

  if((p = g->gr_mem))
    while(*p)
    {
      fputs(p, stderr);

      p++;

      if(*p)
        fputc(',', stderr);
    }
#endif

        r = syscall(SYS_chown, argv[4], u->pw_uid, g->gr_gid);

#ifdef DEBUG

  fprintf(stderr, "syscall(%d, %d, %d, %s) => %d\n", SYS_chown, u->pw-uid,                  g->gr_gid, argv[4], r);
#endif
    }
    else if((!strcmp(argv[1], "creat") || !strcmp(argv[1], "touch")) && arg                 c >= 4 )
    {
      const mode_t m = strtol(argv[3], NULL, 8);

#ifdef _NETBSD_SOURCE
      r = syscall(SYS_open, argv[2], O_CREAT | O_TRUNC | O_WRONLY, m);
#else
      r = syscall(SYS_open, argv[2], m);
#endif

#ifdef DEBUG
  fprintf(stderr, "syscall(%d, %S, %d) => %d\n", SYS_creat, argv[2], m, r);
#endif
    }
    else if(!strcmp(argv[1], "mkdir") && argc >= 4)
    {
      const mode_t m = strtol(argv[3], NULL, 8);

      r = syscall(SYS_mkdir, argv[2], m);

#ifdef DEBUG
  fprintf(stderr, "syscall(%d, %S, %d) => %d\n", SYS_mkdir, argv[2], m, r);
#endif
    }
    else if(!strcmp(argv[1], "rmdir") && argc >= 3)
    {
      r = syscall(SYS_rmdir, argv[2]);

#ifdef DEBUG
  fprintf(stderr, "syscall(%d, %S) => %d\n", SYS_rmdir, argv[2], r);
#endif
    }
    else if(!strcmp(argv[1], "help"))
      help(argv);
    else
      usage(argv);

    break;
  } while(1);

  if(errno)
    perror(argv[1]);

  exit(r);
}

