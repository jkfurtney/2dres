/* cfp  a simple configuration file parsing module
   documentation in cfp.h
   
   this file contains the lex (flex) tokens
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


  /* compile this crap with yacc -d cfp.y &&   flex cfp.l  && gcc lex.yy.
     c y.tab.c -o eg1  or die!

     todo. add a state variable so we count the correct number if line numbers
     in comments.
     
     add support for python style """ block quotes """
     
  */
#include <stdio.h>
#include <string.h>
#include "y.tab.h"


int line_count = 1;

%}

%x comment 
%%

"/*"                   BEGIN(comment);
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n            line_count++;
<comment>"*"+"/"       BEGIN(INITIAL);

#[^\n]*            /* shell style comments */
\".*\"              yylval.str = strdup(yytext);  return QUOTESTRING;
[^\n \t=]+          yylval.str = strdup(yytext);  return KEY;
=                   return EQUALS;
\n                  line_count++; return NEWLINE;
[ \t]*              /* white space */


%%
