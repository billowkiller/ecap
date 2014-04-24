 
#ifndef kW_MATCH_INCLUDED
#define kW_MATCH_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

int kw_match(char *str);
void read_kw_file();
void read_url_file();
int url_match(char *str);
#endif
