#include <stdio.h>
#include <ctype.h>
#include "ptex.h"



enum {TEXT, MATH};

static int c;

static void math (int endmark);
static void content (int endmark);



void
parse (void)
{
    c = next ();
    content (EOF);
    release (EOF);
}



static void
checkpar (int mark)
{
    if (c == mark) return;
    if (c == ')' || c == '}' || c == ']')
    {
        if (mark == ')' || mark == ']')
        {
            fprintf (stderr, "Warning: mismatching '%c'\n", c);
            return;
        }
        release (EOF);
        fprintf (stderr, "Mismatch\n");
        exit (1);
    }
}



static void
skip (int endmark)
{
    while (c != EOF && c != endmark)
    {
        release (c);
        c = next ();
    }
    release (c);
    c = next ();
}



static void
fill (int mode)
{
    int mark;

    while (c != EOF && !isspace (c) && c != '~')
    {
        if (mode == MATH && (c == '-' || c == '_' || c == '^')) return;
        if (c == ')' || c == '}' || c == ']') return;
        else if (c == '(') mark = ')';
        else if (c == '{') mark = '}';
        else if (c == '[') mark = ']';
        else if (c == BSPAR) mark = ENDBSPAR;
        else if (c == BSSQB) mark = ENDBSSQB;
        else if (c == '$') mark = '$';
        else mark = NULLTOKEN;
        release (c);
        c = next ();
        if (mark == ENDBSPAR || mark == ENDBSSQB
        || mark == '$') skip (mark);
        else if (mark != NULLTOKEN)
        {
            if (mode == TEXT || mark == '}') content (mark);
            else math (mark);
            checkpar (mark);
            release (c);
            c = next ();
        }
    }
    while (isspace (c) || c == '~')
    {
        release (c);
        c = next ();
    }
}



static void
formula (void)
{
    int mark;

    mark = c;
    c = next ();
    if (mark == '-' || mark == '=')
    {
        release ('$');
        if (mark == '=') release ('$');
        math (mark);
        release ('$');
        if (mark == '=') release ('$');
    }
    else
    {
        release ('\\');
        if (mark == EQPAR) release ('[');
        else release ('(');
        math (')');
        release ('\\');
        if (mark == EQPAR) release (']');
        else release (')');
    }
    c = next ();
}



static void
command (int mode)
{
    int mark;

    do release (c);
    while (isalpha (c = next ()));
    if (mode == TEXT) while (isspace (c) || c == '~')
    {
        release (c);
        c = next ();
    }
    while (c == '(' || c == '[')
    {
        if (c == '(')
        {
            c = '{';
            mark = ')';
        }
        else mark = ']';
        release (c);
        c = next ();
        if (mode == TEXT) content (mark);
        else math (mark);
        checkpar (mark);
        if (mark == ')') mark = '}';
        release (mark);
        c = next ();
    }
}



static void
script (void)
{
    release (c);
    c = next ();
    if (c == '(')
    {
        release ('{');
        c = next ();
        math (')');
        checkpar (')');
        release ('}');
        c = next ();
    }
}



static void
emphas (void)
{
    srelease ("{\\em ");
    do
    {
        c = next ();
        if (c == '*') break;
        release (c);
    }
    while (c != EOF);
    if (c == '*') c = next ();
    release ('}');
}



static void
math (int endmark)
{
    int prev;

    while (c != EOF && c != endmark)
    {
        if (c == DOT)
        {
            release ('\\');
            c = next ();
            command (MATH);
        }
        else if (c == '^' || c == '_') script ();
        else if (isalpha (c))
        {
            prev = c;
            c = next ();
            if (isalpha (c))
            {
                release ('\\');
                release (prev);
                command (MATH);
            }
            else release (prev);
        }
        else if (c == '-')
        {
            release (c);
            c = next ();
        }
        else fill (MATH);
        checkpar (endmark);
    }
}



static void
content (int endmark)
{
    while (c != EOF && c != endmark)
    {
        if (c == DOT)
        {
            release ('\\');
            c = next ();
            command (TEXT);
        }
        else if (c == DASHPAR || c == EQPAR
        || c == '-' || c == '=')
        {
            formula ();
            if (c == '-') /* TEMPORARY BUG FIX */
            {
                release ('-');
                c = next ();
            }
        }
        else if (c == '*') emphas ();
        else fill (TEXT);
        checkpar (endmark);
    }
}



