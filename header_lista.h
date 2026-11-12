/* Cărăușu Nelu-Rareș-Ionuț  -  323CB */

#ifndef HEADER_LISTA_H
#define HEADER_LISTA_H
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>


typedef struct a {
	void *info;
	struct a *next;
} nod;

typedef struct {
	int ID;
} Thread;

typedef struct {
	int ID;
	Thread *thread;
	uint8_t prioritate;
	uint32_t timpExec;
	uint32_t timpExecInit;
} Task;


#endif