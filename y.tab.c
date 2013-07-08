#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define YYPREFIX "yy"
#line 9 "cfp/cfp.y"
/**********************************************************************
 * Copyright (C) 2002 2003 Jason Furtney                              *
 *                        jkfurtney@hotmail.com                       * 
 *                                                                    *
 *                                                                    *
 * This program is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License        *
 * as published by the Free Software Foundation; either version 2     *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This program is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *      
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this program; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,             *
 * MA 02111-1307, USA.                                                *
 *                                                                    *
 *********************************************************************/


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
#include "cfp/cfp.h"

extern char* yytext;
extern int line_count;
extern FILE *yyin;

#ifndef _CFP_BE
#define _CFP_BE

#define MAXCONFIGOPTS 100

struct keyValuePair {
  char *key;
  char *value;
};
struct cfpData {
  struct keyValuePair data[MAXCONFIGOPTS];
  int verbosity;
  int size;
  int errors;
  int warnings;
};

int cfpSetKeyValuePair(cfpObject c, char *key, char *value);
/* this function returns zero on failure
   positive on success
   and -1 on duplicate keys */

/* global c hack NOT THREAD SAFE */ 
cfpObject _c = NULL;

#endif

int cfpErrors(cfpObject c) {
  return (c->errors);
}

int cfpWarnings(cfpObject c) {
  return (c->warnings);
}

cfpObject cfpNew(int verbosity) {
  /* constructor */
  cfpObject c;
  c = malloc(sizeof(struct cfpData));
  if (verbosity > 2 || verbosity < 0) {fprintf(stderr, 
      "cfp error: cfpNew() must be called with 0, 1, or 2\n"); exit(1);}
  c->verbosity = verbosity;
  c->size = 0; 
  c->errors = 0;
  c->warnings = 0;
  return (c);
}

void cfpDelete(cfpObject c) {
  /* destructor 
     free sections (not implimentd yet)
   */
  int i;
  for (i=0; i<c->size; i++) {
    free(c->data[i].key);
    free(c->data[i].value);
  }
  free(c);
}


int cfpSetKeyValuePair(cfpObject c, char *key, char *value) {
  /* here we set key value pairs into the instances data structure
     for now if a key is repeated we replace the old value with the 
     new one
     
     1  return value means everything went to plan
     -1 indicates duplicate key but OK
     0  indicates failure (due to memory problem)

     todo:
     check that the key has no spaces
     
  */
  int i, debug = 0;
  
  
  if (c->size >= MAXCONFIGOPTS-1) {
    if (c->verbosity > 0){
      fprintf(stderr, "Maximum configuration file size exceeded\n");
      return(0);
    }
  }  
  /* check here that we are not duplicating a key */
  if (cfpIsKeySet(c, key)) {    
    for (i=0; i<c->size; i++) {
      if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
      if (strcmp(key, c->data[i].key) ==  0) {
	/* here we have a duplicate */	
	free(c->data[i].value);
	c->data[i].value = strdup(value);
	return(-1);
      }    
    }	   	     	     	   
  }
  
  
  c->data[c->size].key = strdup(key);
  c->data[c->size].value = strdup(value);
  c->size++;	
  return(1);

}
 
int cfpIsKeySet(cfpObject c, char *key) {
  /* read a cfpObject and return 1 if key exists */
  int debug = 0;
  int i;
  int returnVal = 0;
  char *error = NULL;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {
      return (1);
    }    
  }
  return(0);
}

int cfpIsKeyInt(cfpObject c, char *key) {
  int debug = 0;
  int i;
  int returnVal = 0;
  char *error = NULL;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {
      returnVal = strtol(c->data[i].value, &error, 0);
      if (error[0] == '\0') {
	return (1);
      }else{
	return(0);
      }
    }
  }
  return(0);
}  

int cfpIsKeyDouble(cfpObject c, char *key) { 
  int debug = 0;
  int i;
  char *error = NULL;
  double returnVal;
  
  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {
      returnVal = strtod(c->data[i].value, &error);
      if (error[0] == '\0') {
	return (1);
      }else{
	return(0);
      }
    }
  }
  return (0);
}

int cfpGetInt(cfpObject c, char *key) {
  /* this function reads the global data structure c->data
     looking for key. When key is found the corresponding value
     is converted to an integer. If the key is non-existant or the 
     conversion fails the program  exits with an error message
   */
  int debug = 0;
  int i;
  int returnVal = 0;
  char *error = NULL;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {
      returnVal = strtol(c->data[i].value, &error, 0);

      if (error[0] == '\0') {
	return (returnVal);
      }else{
	fprintf(stderr, "Error while parsing configuration file: value %s is not a valid integer\n", c->data[i].value);
	exit(1);
      }
    }
  }
  fprintf(stderr, "Error while parsing configuration file: key %s is missing\n", key);
  exit(1);
}

double cfpGetDouble(cfpObject c, char *key) {
  /* this function reads the global data structure c->data
     looking for key. When key is found the corresponding value
     is converted to an double. If the key is non-existant or the conversion
     fails the program exits with an error message
   */

  int debug = 0;
  int i, mat;
  double returnVal = 0;
  char *error = NULL;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {
      

      if (debug) {printf("string to be converted is: %s\n", c->data[i].value);}
      
      returnVal = strtod(c->data[i].value, &error);
      
      if (error[0] == '\0') {
	return (returnVal);
      }else{
	fprintf(stderr, "Error while parsing configuration file: value %s is not a valid double \n", c->data[i].value);
	exit(1);
      }
    }
  }
  fprintf(stderr, "Error while parsing configuration file: key %s is missing\n", key);
  exit(1);
}

char *cfpGetString(cfpObject c, char *key) {
  /* this function reads the global data structure c->data
     looking for key. When key is found the corresponding value
     is returned as a string. If the key does not exist the program
     exits with an error
   */

  int debug = 0;
  int i;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {      
      return strdup(c->data[i].value);      
    }
  }
  fprintf(stderr, "Error while parsing configuration file: key %s is missing\n", key);
  exit(1);
}

int cfpGetBoolean(cfpObject c, char *key) {
  /* this function reads the global data structure c->data looking for
     key. When key is found the corresponding value is returned as 1
     or 0 corresponding to the boolian value. If the key does not
     exist or is an invlid boolian string the program exits with an
     error 
  */

  int debug = 0;
  int i;
  
  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {      
      /* true conditions */
      if (strcmp(c->data[i].value, "TRUE") == 0) { return (1);}
      if (strcmp(c->data[i].value, "True") == 0) { return (1);}
      if (strcmp(c->data[i].value, "true") == 0) {return (1); }
      if (strcmp(c->data[i].value, "1") == 0) { return (1);}
      if (strcmp(c->data[i].value, "on") == 0) {return (1);}
      if (strcmp(c->data[i].value, "On") == 0) {return (1);}
      if (strcmp(c->data[i].value, "ON") == 0 ) {return (1);}
      
      /*false conditioins */
      if (strcmp(c->data[i].value, "FALSE") == 0) { return (0);}
      if (strcmp(c->data[i].value, "False") == 0) { return (0);}
      if (strcmp(c->data[i].value, "false") == 0) {return (0); }
      if (strcmp(c->data[i].value, "0") == 0) { return (0);}
      if (strcmp(c->data[i].value, "off") == 0) {return (0);}
      if (strcmp(c->data[i].value, "Off") == 0) {return (0);}
      if (strcmp(c->data[i].value, "OFF") == 0 ) {return (0);}
      
      fprintf(stderr, "cfp error: unrecognised boolian value %s. Value must be one of TRUE True true 1 on On ON FALSE False false 0 off Off OFF\n", c->data[i].value);
      
    }
  }
  fprintf(stderr, "Error while parsing configuration file: key %s is missing\n", key);
  exit(1);
}
 

int cfpIsKeyBoolean(cfpObject c, char *key) {
  /* this function reads the global data structure c->data looking for
     key. When key is found the corresponding value is returned as 1
     or 0 corresponding to the boolian value. If the key does not
     exist or is an invlid boolian string the program exits with an
     error 
  */

  int debug = 0;
  int i;

  for (i=0; i<c->size; i++) {
    if (debug) {printf("%i of  %i -- %s %s\n", c->size, i, key, c->data[i].key );}
    if (strcmp(key, c->data[i].key) ==  0) {      
      /* true conditions */
      if (strcmp(c->data[i].value, "TRUE") == 0) { return (1);}
      if (strcmp(c->data[i].value, "True") == 0) { return (1);}
      if (strcmp(c->data[i].value, "true") == 0) {return (1); }
      if (strcmp(c->data[i].value, "1") == 0) { return (1);}
      if (strcmp(c->data[i].value, "on") == 0) {return (1);}
      if (strcmp(c->data[i].value, "On") == 0) {return (1);}
      if (strcmp(c->data[i].value, "ON") == 0 ) {return (1);}
      
      /*false conditioins */
      if (strcmp(c->data[i].value, "FALSE") == 0) { return (1);}
      if (strcmp(c->data[i].value, "False") == 0) { return (1);}
      if (strcmp(c->data[i].value, "false") == 0) {return (1); }
      if (strcmp(c->data[i].value, "0") == 0) { return (1);}
      if (strcmp(c->data[i].value, "off") == 0) {return (1);}
      if (strcmp(c->data[i].value, "Off") == 0) {return (1);}
      if (strcmp(c->data[i].value, "OFF") == 0 ) {return (1);}
      
      return(0); /* key is set but not valid boolian */
    }    
  }
  /* key is not set */
  return(0);
}

int cfpParseFile(cfpObject c, char *filename) {
  /* this function reads the file filename and populates the global
     data structure c->data. Errors are reported during parsing.
     return values:
     returnValue == -1 problem opening the file
     returnValue < -1 the number of errors is returned as numOfErr*-1 + -1
     returnValue <= 0 the number of key value pairs parsed
   */
  int debug = 1;
  int count;
  FILE *configFilePtr;
  count =  0;

  configFilePtr = fopen(filename, "r");

  if (configFilePtr == NULL) {
    if (c->verbosity > 0){
      fprintf(stderr, "Error while parsing configuration file:\n");
      fprintf(stderr, "Can't open file %s :\n", filename);
    }
    return(0);
  }

  yyin = configFilePtr;
  _c = c;  /* global use to work with yacc */
  yyparse();
  _c = NULL;
  return (c->size); 
}

void yyerror(const char *str) {
  _c->errors++;
  if(_c->verbosity > 0) { 
    fprintf(stderr, "error line %i: %s\n", line_count,  str);
  }
}

int yywrap() { return (1);}


#line 405 "cfp/cfp.y"
typedef union {
  char *str;
  int i;
} YYSTYPE;
#line 417 "y.tab.c"
#define YYERRCODE 256
#define EQUALS 257
#define NEWLINE 258
#define KEY 259
#define QUOTESTRING 260
const short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    4,    3,    2,
};
const short yylen[] = {                                         2,
    0,    2,    1,    1,    2,    1,    1,    4,    4,
};
const short yydefred[] = {                                      1,
    0,    0,    7,    0,    2,    3,    4,    6,    5,    0,
    0,    0,    9,    8,
};
const short yydgoto[] = {                                       1,
    5,    6,    7,    8,
};
const short yysindex[] = {                                      0,
 -256, -257,    0, -251,    0,    0,    0,    0,    0, -255,
 -250, -249,    0,    0,
};
const short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
const short yygindex[] = {                                      0,
    0,    0,    0,    5,
};
#define YYTABLESIZE 9
const short yytable[] = {                                       2,
    3,    3,    4,   11,   12,   10,    9,   13,   14,
};
const short yycheck[] = {                                     256,
  258,  258,  259,  259,  260,  257,    2,  258,  258,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 260
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"EQUALS","NEWLINE","KEY",
"QUOTESTRING",
};
const char * const yyrule[] = {
"$accept : commands",
"commands :",
"commands : commands command",
"command : keyValueSet",
"command : keyQuoteString",
"command : error nl",
"command : nl",
"nl : NEWLINE",
"keyQuoteString : KEY EQUALS QUOTESTRING NEWLINE",
"keyValueSet : KEY EQUALS KEY NEWLINE",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 7:
#line 429 "cfp/cfp.y"
{
	 yyerrok;  /* lets have lots of error messages please */
}
break;
case 8:
#line 435 "cfp/cfp.y"
{
	 char *tmp;	 
	 int i;

	 /* cut out the quotes */
	 tmp = (char *)malloc(sizeof(char)*(strlen(yyvsp[-1].str)-2));
	 strncat(tmp, ((char *)yyvsp[-1].str+1),strlen(yyvsp[-1].str)-2);

	 if (_c->verbosity > 1){
	   printf("found  Key: %s Value: %s\n", yyvsp[-3].str, tmp); 
	 }	 
	 
	 i = cfpSetKeyValuePair(_c, yyvsp[-3].str, tmp);

	 if (i == 0) {exit(1);}
	 if (i == -1) { 
	   _c->warnings++;
	   if (_c->verbosity > 0){ 
	     fprintf(stderr, "cfp warning: duplicate key: %s found on line %i using _new_ value: %s  \n ", yyvsp[-3].str, line_count - 1,  yyvsp[-1].str);          
	   }
	 }	 
       }
break;
case 9:
#line 460 "cfp/cfp.y"
{
	 int i;
	 
	 if (_c->verbosity > 1){
	   printf("found  Key: %s Value: %s\n", yyvsp[-3].str, yyvsp[-1].str); 
	 }
	 
	 i = cfpSetKeyValuePair(_c, yyvsp[-3].str, yyvsp[-1].str);

	 if (i == 0) {exit(1);}
	 if (i == -1) { 
	   _c->warnings++;
	   if (_c->verbosity > 0){ 
	     fprintf(stderr, "cfp warning: duplicate key: %s found on line %i using _new_ value: %s  \n ", yyvsp[-3].str, line_count - 1 ,  yyvsp[-1].str);
	   }	   
	 }	 
       }
break;
#line 755 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
