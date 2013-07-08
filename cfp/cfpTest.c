/* cfp a simple configuration file parsing module
   
   example program for cfp module 
   see cfp.h for full documentation
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

#include<stdio.h>
#include "cfp.h"

int main (void) {

  int i=1;
  int j, kvp;
  double b;
  char *ss;
  cfpObject a = cfpNew(1);

  if (-1 == (i = cfpParseFile(a, "./testConfigFile"))) {
    fprintf(stderr, "error opening file exiting...\n"); exit(1);}
  
  printf("found %i key value pairs in the configuration file with %i parse error(s) and %i warning(s)\n", i, cfpErrors(a), cfpWarnings(a));

  if (cfpIsKeyInt(a, "jason")) {
    j = cfpGetInt(a, "jason");    
    printf("key jason has value %i\n", j);    
  }

  i = cfpGetBoolean (a, "jason");
  printf("key jason has value %i\n", i);    

  i = cfpGetBoolean (a, "jj");
  printf("key jj has value %i\n", i);    
    
  ss = cfpGetString(a, "kerry"); 
  printf("key kerry has value %s\n", ss);    
  
  cfpSetKeyValuePair(a, "hihi", "bbbasbadbsada");
  ss = cfpGetString(a, "key1"); 
  printf("key key1 has value %s\n", ss);    
    
  b = cfpGetDouble(a, "bbb");
  printf("key bbb has value %5.5f\n", b);    


  b = cfpGetDouble(a, "key2");
  printf("key key2 has value %5.5f\n", b);    

  printf("key test (0) : %d \n", cfpIsKeySet(a, "dsfsdf2")); 
  printf("key test (0) : %d \n", cfpIsKeyInt(a, "dsfsdf2"));
  printf("key test (0) : %d \n", cfpIsKeyDouble(a, "dsfsdf2"));
  printf("key test (1) : %d \n", cfpIsKeySet(a, "key2"));
  printf("key test (0) : %d \n", cfpIsKeyInt(a, "key2"));
  printf("key test (1) : %d \n", cfpIsKeyDouble(a, "key2"));
  printf("key test (1) : %d \n", cfpIsKeySet(a, "jason")); 
  printf("key test (1) : %d \n", cfpIsKeyInt(a, "jason"));   
  printf("key test (1) : %d \n", cfpIsKeyDouble(a, "jason"));   
  printf("key test (0) : %d \n", cfpIsKeyDouble(a, "keyy"));   
  
  cfpDelete(a);
  return (0);
}

