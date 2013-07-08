/* This module impliments a simple configuration file parsing system
   full documentation in cfp.h
   
   this file contain the yacc grammar and the functions definitions for
   manipulating the data.

 */
%{
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


%}

%union {
  char *str;
  int i;
};

%token  EQUALS  NEWLINE 
%token <str> KEY QUOTESTRING

%%

commands:
        | commands command
        ;

command: 
       keyValueSet
       |
       keyQuoteString
       |
       error nl 
       |
       nl 
       ;

nl: NEWLINE {
	 yyerrok;  /* lets have lots of error messages please */
};

keyQuoteString:
       KEY EQUALS QUOTESTRING NEWLINE
       {
	 char *tmp;	 
	 int i;

	 /* cut out the quotes */
	 tmp = (char *)malloc(sizeof(char)*(strlen($3)-2));
	 strncat(tmp, ((char *)$3+1),strlen($3)-2);

	 if (_c->verbosity > 1){
	   printf("found  Key: %s Value: %s\n", $1, tmp); 
	 }	 
	 
	 i = cfpSetKeyValuePair(_c, $1, tmp);

	 if (i == 0) {exit(1);}
	 if (i == -1) { 
	   _c->warnings++;
	   if (_c->verbosity > 0){ 
	     fprintf(stderr, "cfp warning: duplicate key: %s found on line %i using _new_ value: %s  \n ", $1, line_count - 1,  $3);          
	   }
	 }	 
       }

keyValueSet:
       KEY EQUALS KEY NEWLINE
       {
	 int i;
	 
	 if (_c->verbosity > 1){
	   printf("found  Key: %s Value: %s\n", $1, $3); 
	 }
	 
	 i = cfpSetKeyValuePair(_c, $1, $3);

	 if (i == 0) {exit(1);}
	 if (i == -1) { 
	   _c->warnings++;
	   if (_c->verbosity > 0){ 
	     fprintf(stderr, "cfp warning: duplicate key: %s found on line %i using _new_ value: %s  \n ", $1, line_count - 1 ,  $3);
	   }	   
	 }	 
       }
       ;
