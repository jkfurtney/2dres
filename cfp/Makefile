#**********************************************************************
# * Copyright (C) 2002 2003 Jason Furtney                              *
# *                        jkfurtney@hotmail.com                       * 
# *                                                                    *
# *                                                                    *
# * This program is free software; you can redistribute it and/or      *
# * modify it under the terms of the GNU General Public License        *
# * as published by the Free Software Foundation; either version 2     *
# * of the License, or (at your option) any later version.             *
# *                                                                    *
# * This program is distributed in the hope that it will be useful,    *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of     *      
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
# * GNU General Public License for more details.                       *
# *                                                                    *
# * You should have received a copy of the GNU General Public License  *
# * along with this program; if not, write to the Free Software        *
# * Foundation, Inc., 59 Temple Place - Suite 330, Boston,             *
# * MA 02111-1307, USA.                                                *
# *                                                                    *
# *********************************************************************/

SHELL=/bin/sh
CC = gcc
CFLAGS = -g 

cfpTest: y.tab.o lex.yy.o cfpTest.c cfp.h 
	${CC} ${CFLAGS} -o cfpTest cfpTest.c y.tab.o lex.yy.o

y.tab.o: y.tab.c  cfp.h  
	${CC} ${CFLAGS} -c   y.tab.c 

lex.yy.o: lex.yy.c cfp.h  
	${CC} ${CFLAGS} -c  lex.yy.c	

lex.yy.c: cfp.l	
	flex cfp.l

y.tab.c: cfp.y 	
	yacc -d cfp.y


