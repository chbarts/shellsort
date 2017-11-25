#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _GNU_SOURCE             /* Enable getopt_long(). */
#include <getopt.h>
#include "handle_ferr.h"
#include "shellsort.h"
#include "ggets.h"

static int rev = 1;

static int ncmp(void *left, void *right)
{
    long long int ln, rn;
    char *endl = NULL, *endr = NULL, *lft = (char *) left, *rht =
        (char *) right;
    int res;

    ln = strtol(lft, &endl, 10);

    if ((0 == ln) && (endl == lft)) {
        return rev * strcmp(lft, rht);
    }

    rn = strtol(rht, &endr, 10);

    if ((0 == rn) && (endr == rht)) {
        return rev * strcmp(lft, rht);
    }

    if (ln < rn) {
        res = -1;
    } else if (ln > rn) {
        res = 1;
    } else {
        res = strcmp(endl, endr);
    }

    return rev * res;
}

static int cmp(void *left, void *right)
{
    char *lft = (char *) left, *rht = (char *) right;
    return rev * strcmp(lft, rht);
}

static void usage(char *pname)
{
    printf("Usage: %s [OPTION] [FILE...]\n", pname);
    puts("Sort files or, if no files on the command line, sort stdin.");
    puts("Multiple sorting criteria can be combined.");
    puts("All files sorted together into one output.");
    puts("");
    puts("  -r, --reverse     Reverse the sense of the comparison");
    puts("  -n, --numer       Sort numerically, then non-numerically");
    puts("  -u, --uniq        Uniqify output");
    puts("  -h, --help        Print this help");
    puts("  -v, --version     Print version information");
    puts("");
    puts("Report bugs to <chbarts@gmail.com>.");
}

static void version(void)
{
    puts("ssort version 0.1.0");
    puts("Copyright 2017 Chris Barts.");
    puts("sort comes with NO WARRANTY to the extent permitted by law.");
    puts("You may redistribute this software under the terms");
    puts("of the GNU General Public License.");
    puts("For more information, see the file named COPYING");
    puts("in this program's distribution file.");
}

static char **buf;
static size_t curlen = 0, curmax = 0;

static void dofile(char *fname, FILE * fin)
{
    char **tmp, *ln;
    size_t i;

    while (fggets(&ln, fin) == 0) {
        if (curlen == curmax) {
            tmp = realloc(buf, curmax * 2 * sizeof(buf[0]));
            if (NULL == tmp) {
                fclose(fin);
                for (i = 0; i < curmax; i++)
                    free(buf[i]);
                free(buf);
                fprintf(stderr, "ssort: memory error\n");
                exit(EXIT_FAILURE);
            } else {
                curmax *= 2;
                buf = tmp;
            }
        }

        buf[curlen++] = ln;
    }

    if (ferror(fin) != 0)
        handle_ferr(fname, "ssort");
}

int main(int argc, char *argv[])
{
    size_t j;
    char *pname, *prev = NULL;
    FILE *fin;
    int i, c, lind, numer = 0, uniq = 0;
    extern int optind;
    struct option longopts[] = {
        {"reverse", 0, 0, 0},
        {"numer", 0, 0, 0},
        {"uniq", 0, 0, 0},
        {"help", 0, 0, 0},
        {"version", 0, 0, 0},
        {0, 0, 0, 0}
    };

    if ((buf = malloc(1024 * sizeof(buf[0]))) == NULL) {
        fprintf(stderr, "ssort: malloc failure\n");
        exit(EXIT_FAILURE);
    }

    curmax = 1024;

    if (1 == argc) {
        dofile("stdin", stdin);
        shellsort((void **) buf, curlen, cmp);
        for (j = 0; j < curlen; j++) {
            puts(buf[j]);
            free(buf[j]);
        }

        free(buf);
        exit(EXIT_SUCCESS);
    }

    pname = argv[0];

    while ((c = getopt_long(argc, argv, "rnuhv", longopts, &lind)) != -1) {
        switch (c) {
        case 0:
            switch (lind) {
            case 0:
                rev = -1;
                break;
            case 1:
                numer = 1;
                break;
            case 2:
                uniq = 1;
                break;
            case 3:
                usage(pname);
                exit(EXIT_SUCCESS);
                break;
            case 4:
                version();
                exit(EXIT_SUCCESS);
                break;
            default:
                usage(pname);
                exit(EXIT_FAILURE);
                break;
            }

            break;

        case 'r':
            rev = -1;
            break;
        case 'n':
            numer = 1;
            break;
        case 'u':
            uniq = 1;
            break;
        case 'h':
            usage(pname);
            exit(EXIT_SUCCESS);
            break;
        case 'v':
            version();
            exit(EXIT_SUCCESS);
            break;
        default:
            usage(pname);
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (optind >= argc) {
        dofile("stdin", stdin);
    } else {
        for (i = optind; i < argc; i++) {
            if ((fin = fopen(argv[i], "rb")) == NULL) {
                handle_ferr(argv[i], "ssort");
                continue;
            }

            dofile(argv[i], fin);

            if (fclose(fin) == EOF) {
                handle_ferr(argv[i], "ssort");
            }
        }
    }

    if (0 == curlen) {
        free(buf);
        return 0;
    }

    if (1 == numer) {
        shellsort((void **) buf, curlen, ncmp);
    } else {
        shellsort((void **) buf, curlen, cmp);
    }

    for (j = 0; j < curlen; j++) {
        if (1 == uniq) {
            if (0 == j) {
                puts(buf[j]);
                prev = buf[j];
            } else {
                if (strcmp(prev, buf[j]))
                    puts(buf[j]);
                free(prev);
                prev = buf[j];
            }

        } else {
            puts(buf[j]);
            free(buf[j]);
        }
    }

    free(prev);
    free(buf);

    return 0;
}
