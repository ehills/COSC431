/*
  Regular Expression tester.

  When learning/developing a regular expression, you could use grep,
  which has nice colouring support, but unfortunately, the highlighting
  shows _all_ the matching parts, which can be misleading (eg. with
  a regular expression of '.', the input 'hello' is shown as all matching.

  Cameron Kerr <ckerr@cs.otago.ac.nz>
  February 2011
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>

int regcomp_flags = REG_EXTENDED;

void usage(void)
{
  fprintf(stderr,
          "Regular Expression Tester\n"
          "  Matches text on stdin against the supplied regexp\n"
          "  Prints out ONLY the part that matched, or the text <NO MATCH>\n"
          "\n"
          "Usage: regex_test [-e|-b] [-i] <regex>\n"
          "   -e  Use POSIX Extended regular expressions (default)\n"
          "   -b  Use POSIX Basic regular expressions\n"
          "   -i  Case-insensitive match\n");
  exit(EXIT_FAILURE);
}

void chomp(char *s)
{
  int len = strlen(s);
  if (s[len-1] == '\n')
    s[len-1] = '\0';
}

int main(int argc, char *argv[])
{
  int option;
  regex_t regex;
  int regerror_code;
  static regmatch_t regex_regions[1];
  static char errbuf[128];
  static char buf[1024];

  /* Enable Localisation support */
  if (setlocale(LC_ALL, "") == NULL) {
    fprintf(stderr, "Failed to set specified locale!\n");
    exit(EXIT_FAILURE);
  }

  while ((option = getopt(argc, argv, "ebih")) != -1) {
    switch (option) {

    case 'e':
      regcomp_flags |= REG_EXTENDED;
      break;

    case 'b':
      regcomp_flags &= ~REG_EXTENDED;
      break;

    case 'i':
      regcomp_flags |= REG_ICASE;
      break;

    default:
      usage();
      break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected regular expression!\n");
    usage();
  }

  if ((regerror_code = regcomp(&regex, argv[optind], regcomp_flags)) != 0) {
    regerror(regerror_code, &regex, errbuf, sizeof errbuf);
    fprintf(stderr, "Problem with regular expression: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  while (fgets(buf, sizeof buf, stdin) != NULL) {
    chomp(buf);

    regerror_code = regexec(&regex, buf, 1, regex_regions, 0);

    switch (regerror_code) {
    case 0:
      fwrite(buf, 1, regex_regions[0].rm_so, stdout);
      printf("\033[01;31m\033[K");
      fwrite(buf + regex_regions[0].rm_so, 1,
             regex_regions[0].rm_eo - regex_regions[0].rm_so, stdout);
      printf("\033[m\033[K");
      fwrite(buf + regex_regions[0].rm_eo, 1,
             strlen(buf) - regex_regions[0].rm_eo, stdout);
      printf("\n");
      
      if (regex_regions[0].rm_eo - regex_regions[0].rm_so == 0) {
        printf("  (successfully matched 0 characters from index %d to %d)\n",
               regex_regions[0].rm_so, regex_regions[0].rm_eo);
      }
      break;

    case REG_NOMATCH:
      printf("<NO MATCH>\n");
      break;

    default:
      fprintf(stderr, "Unexpected error running regexec: %d\n", regerror_code);
      exit(EXIT_FAILURE);
      break;
    }
  }

  regfree(&regex);

  return 0;
}
