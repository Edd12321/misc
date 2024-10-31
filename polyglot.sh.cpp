#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#define then {
#define fi ;}
#define for(x) for x
#define do {
#define done ;}
#define echo std::cout << std::endl <<
#define function int
#define true int
#define $i << i;
#define cat 1
#define a 0
#define sleep 0+
#define false 0;
#define uname (({pid_t pid = fork(); if (!pid) { char *x[] = {"uname", "-a", NULL}; execvp(*x, x); } else { while (wait(NULL) > 0); }}), 1)
true i;

function main ()
{
    //bin/echo
    if (( 1 == 1 ))
    then
        for ((i=1;i<=10;++i))
        do
            echo "I love bash!";
            echo "i = "$i;
        done
    fi

    (echo "hi mom" && uname -a | cat);
    sleep 1 & false
}

/* 2> /dev/null
main
2> /dev/null */
