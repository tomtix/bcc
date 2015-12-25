#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "program.h"
#include "module.h"
#include "function.h"
#include "option.h"

#include "symbol/symbol_table.h"
#include "error/error.h"

int compile_file(const char *filename, struct bcc_option *bopt);
int yyparse(void);
extern FILE *yyin;

char *yyfilename = NULL;

int main(int argc, char *argv[])
{
    const struct type *arrt = type_new_array_type(
        type_int,
        expr_constant(constant_integer_int(0)));

    arrt = type_new_array_type(type_new_function_type(type_float, list_new(0)),
                               expr_constant(constant_integer_int(10)));
    arrt = type_new_array_type(arrt,
                               expr_constant(constant_integer_int(2)));
    
    printf("%s\n", type_printable(arrt));
    exit(EXIT_SUCCESS);
    
    
    struct bcc_option bopt ;//= { 0 };
    int err = 0;

    if (1 == argc) {
        yyfilename = *argv;
        internal_error("no input files\n"); // exits
        // never reached
        fprintf(stderr, "compilation terminated\n");
        exit(EXIT_FAILURE);
    }

    parse_options(&argc, argv, &bopt);
    int i = 1;
    while (i < argc) {
        FILE *input =  fopen(argv[i], "r");
        if (NULL == input) {
            perror(argv[i]);
            exit(EXIT_FAILURE);
        }
        
        char *mname = strdup(argv[i]);
        yyfilename = mname;
        st_init();
        m = module_new(mname);

        yyin = input;
        yyparse();

        err = error_count();
        if (!err)
            module_print(m, stdout);
        else
            break;
        i++;
    }

    return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
