/* 
    cfp  -- a configuration file parser

This module impliments a simple configuration file parsing system

*/

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

/*
 Using
 -----

Configuration files have the following format:

key = value 
bool = False
keyMustBeOneWord = "If values are to have spaces they must have double quotes"

/*
  multiline
   c style comments can be used   
*/                                                                          /*

#comment ending at new line
dt = 0.002  # comment
...

values are stored internally as strings (CASE SENSITIVE!) but can be
receved as strings, integers, or doubles with the functions provided.

 Errors
 ------

Depending on verbosity _non fatal_ errors are reported to stderr during parsing
   these include: duplicate keys, keys with no values,
   = with no key, and more than one = etc..
   
   -- in the case of duplicate keys the new value is used
   
The following are fatal errors that cause an error message to be 
writen to stderr and the program to exit:

1)   the value for the key requested is not present
2)   a problem occurs when converting the requested value to int or float

 Error Checking
 --------------
 the cfpIsKey[Set|Int|Double]() functions are generously provided
 so that the user can test to see that keys are present and well formed

 Example Use
 ------- ---
 see cfpTest.c for a sample of all the features
    
}

        Jason Furtney Jan 2003 jkfurtney@hotmail.com */
#ifndef _CFP_FE
#define _CFP_FE

typedef struct cfpData *cfpObject;

cfpObject cfpNew(int verbosity); /* constructor method 
pass 0 for silence -- only fatal errors are reported
pass 1 for warnings and errors --
pass 2 for verbose parse mode + all warnings and errors
 */

void cfpDelete(cfpObject c); /* destructor 
call to free cfp data structure */

int cfpParseFile(cfpObject c, char *filename);
/*     return value: zero indicates an error opening the specified file
       positive value means things are OK  */

int cfpErrors(cfpObject c);
/* returns the number of errors during parsing 
   must be called after cfpParseFile() 
*/

int cfpGetInt(cfpObject c, char *key);
double cfpGetDouble(cfpObject c, char *key);
int cfpGetBoolean(cfpObject c, char *key);
char *cfpGetString(cfpObject c, char *key);

/* the strings returned by cfpGetString() are alocated by strdup()
   so they can be accessed after cfpDelete() is called
   they can be released to the system with free() at any time.
 */

/* The following functions are key tests  they return 1 or 0
   
   use these functions  to do your own error checking
   (to stop cfp from exiting when an error occurs)
*/

int cfpIsKeySet(cfpObject c, char *key);
int cfpIsKeyInt(cfpObject c, char *key);
int cfpIsKeyBoolean(cfpObject c, char *key);
int cfpIsKeyDouble(cfpObject c, char *key);

#endif
