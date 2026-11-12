/* Cărăușu Nelu-Rareș-Ionuț  -  323CB */

#ifndef HEADER_COADA_H
#define HEADER_COADA_H

#define NR_MAX_TASKURI 32768

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "header_lista.h"


typedef struct {
	nod *head;
	nod *tail;
	int lungime;
} coada;


#endif