#ifndef AC_H
#define AC_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

void initial_goto(GList* g_kw_list);
void buildFail();
int ac(char* s);//input main string
#endif