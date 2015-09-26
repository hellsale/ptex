#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "ptex.h"



static FILE *input;
static FILE *output;

static int opt_expand = TRUE;
static int opt_justify = TRUE;



void
format (char *buf, int *hp, int *tp)
{
    int c, i;

    int head = *hp;
    int tail = *tp;

    if (opt_justify) while (head != tail)
    {
        c = buf [head++];
        head %= BUFSIZ;
        if (c == '\t') for (i = 0; i < 8; i++) putc (' ', output);
        else putc (c, output);
    }
    else while (head != tail)
    {
        putc (buf [head++], output);
        head %= BUFSIZ;
    }
    *hp = head;
    *tp = tail;
}



void
release (int c)
{
    static char buf [BUFSIZ];
    static int head = 0;
    static int tail = 0;

    if ((((tail + BUFSIZ) - head) % BUFSIZ > 255) || c == EOF)
    {
        format (buf, &head, &tail);
    }
    if (c != EOF)
    {
        switch (c)
        {
            case DOT:
                c = '.';
                break;
            case BSPAR: case ENDBSPAR: case BSSQB:
            case ENDBSSQB:
                buf [tail++] = '\\';
                break;
            case DASHPAR: case DBLDASH:
                buf [tail++] = '-'; /* next() doesn't recognize DBLDASH */
                break;
        }
        switch (c)
        {
            case BSPAR: case DASHPAR:
                c = '('; break;
            case DBLDASH: c = '-'; break;
            case ENDBSPAR: c = ')'; break;
            case BSSQB: c = '['; break;
            case ENDBSSQB: c = ']'; break;
        }
        buf [tail++] = (char) c;
        tail %= BUFSIZ;
    }
}



void
srelease (char *s)
{
    while (*s != 0) release (*(s++));
}



void
skipline (void)
{
    char buf [MAXLEN];
    int protect;

    release (EOF);
    do
    {
        fgets (buf, MAXLEN, input);
        fputs (buf, output);
        if (strstr (buf, " protect") == buf) protect = TRUE;
        if (strstr (buf, "% end ") == buf) protect = FALSE;
    }
    while (protect);
}



int
next (void)
{
    static int prev = NULLTOKEN;
    int c;

    if (prev != NULLTOKEN)
    {
        c = prev;
        prev = NULLTOKEN;
    }
    else c = getc (input);
    if (c == '%')
    {
        release ('%');
        skipline ();
        return ('\n');
    }
    else if (c == '.')
    {
        c = getc (input);
        prev = c;
        if (isalpha (c)) return DOT;
        else return '.';
    }
    else if (c == '-')
    {
        c = getc (input);
        if (c == '(') return DASHPAR;
        prev = c;
        return '-';
    }
    else if (c == '=')
    {
        c = getc (input);
        if (c == '(') return EQPAR;
        else if (c == '<') return LEQ;
        else if (c == '>') return GEQ;
        prev = c;
        return '=';
    }
    else if (c == '\\')
    {
        c = getc (input);
        if (c == '(') return BSPAR;
        else if (c == ')') return ENDBSPAR;
        else if (c == '[') return BSSQB;
        else if (c == ']') return ENDBSSQB;
        prev = c;
        return '\\';
    }
    else return c;
}



int
filesetup (char *path, char *buf, char **namep, char **extp, int size)
{
    char *ext;

    strncpy (buf, path, size);
    buf [size - MAXEXT] = 0;
    for (ext = buf; *ext != 0; ext++);
    input = fopen (buf, "r");
    if (input == NULL)
    {
        strcat (buf, PTXEXT);
        input = fopen (buf, "r");
        *extp = NULL;
    }
    if (input == NULL)
    {
        *ext = 0;
        strcat (buf, ".tex");
        if ((input = fopen (buf, "r")) == NULL) return 1;
        for (*extp = ext; **extp != 0; (*extp)++);
    }
    if (output == NULL)
    {
        if (*extp == NULL)
        {
            *ext = 0;
            strcat (buf, RESEXT);
        }
        else strcpy (*extp, TMPEXT);
        if ((output = fopen (buf, "w")) == NULL)
        {
            fclose (input);
            return 1;
        }
    }
    return 0;
}



int
fileclear (char *buf, char *name, char *ext, int backup)
{
    char tmpbuf [BUFSIZ];

    fclose (input);
    if (output != stdout)
    {
        fclose (output);
        output = NULL;
    }
    if (ext != NULL)
    {
        *ext = 0;
        strcpy (tmpbuf, buf);
        if (backup)
        {
            strcpy (ext, BAKEXT);
            if (rename (tmpbuf, buf)) return 1;
        }
        else if (remove (tmpbuf)) return 1;
        strcpy (ext, TMPEXT);
        return rename (buf, tmpbuf);
    }
    return 0;
}



int
main (int argc, char *argv [])
{
    char buf [BUFSIZ];
    char *arg;
    char *name;
    char *extension;
    int backup = FALSE;
    char opt;

    output = NULL;
    while ((opt = getopt (argc, argv, "abefjx")) != EOF)
    {
        switch (opt)
        {
            case 'b':   backup = TRUE; break;
            case 'x':   backup = FALSE; break;
            case 'f':   output = stdout; break;
            case 'j':   opt_expand = FALSE; break;
            case 'e':   opt_justify = FALSE; break;
            case 'a':   opt_expand = TRUE; opt_justify = TRUE; break;
            case '?':   fprintf (stderr,
                "usage: %s [-abefjx] [target...]\n", argv [0]);
                        return 1;
        }
    }
    if (argc == optind)
    {
        input = stdin;
        output = stdout;
        parse ();
    }
    else do
    {
        arg = argv [optind];
        if (filesetup (arg, buf, &name, &extension, BUFSIZ) == 0)
        {
            parse ();
            fileclear (buf, name, extension, backup);
        }
        else
        {
            fprintf (stderr, "%s: %s: file error\n",
                argv [0], arg);
        }
    }
    while (++optind < argc);
    return 0;
}
