#include<stdio.h>
#include"analyzer.h"

int main(int argc,char *argv[])
{
    printf("\t\t::Lexical Analyzer::\n");
    if(argc!=2)
    {
        printf("More than 1 argument is required \n");
        return 0;
    }
    else
    {
         return do_analyze(argc, argv); // Call and return the result of do_analyze
    }
}