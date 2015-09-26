#define TRUE 1
#define FALSE 0

#define TMPEXT ".ptex_int"
#define PTXEXT ".ptex"
#define RESEXT ".p.tex"
#define BAKEXT ".bak"
#define MAXEXT 9

#define MAXLEN 256

enum
{
    NULLTOKEN = 256,
    DOT, DASHPAR, EQPAR, DBLDASH,
    LEQ, GEQ, BSPAR, ENDBSPAR, BSSQB, ENDBSSQB
};

void release (int c);
void srelease (char *s);
int next (void);

void parse (void);

