//
// Created by kr2 on 6/5/21.
//

#ifndef CREPL_STAT_H
#define CREPL_STAT_H

#define bool int
#define true 1
#define false 0

#define COMPILER "gcc"

extern char global_file[];
extern bool isfunction(char *);
extern int interpret(char *);
extern bool compile(char *, char *); // requires function

#endif //CREPL_STAT_H
