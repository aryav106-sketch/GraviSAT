#include <jni.h>
#include "dimacs_parser.h"
#include "solver.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_in_gravisat_MainActivity_runNativeSATSolver(
        JNIEnv* env,
        jobject /* this */) {

    CNFFormula formula;

    formula.variables = 3;

    Clause c1;
    c1.literals = {1, -2};

    Clause c2;
    c2.literals = {2, 3};

    formula.clauses.push_back(c1);
    formula.clauses.push_back(c2);

    Solver solver(formula);

    bool result = solver.solve();

    const char* output;

    if (result)
        output = "SATISFIABLE";
    else
        output = "UNSATISFIABLE";

    return env->NewStringUTF(output);
}
