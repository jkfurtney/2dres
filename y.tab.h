#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define EQUALS 257
#define NEWLINE 258
#define KEY 259
#define QUOTESTRING 260
typedef union {
  char *str;
  int i;
} YYSTYPE;
extern YYSTYPE yylval;
