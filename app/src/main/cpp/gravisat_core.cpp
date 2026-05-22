Solver solver;

bool result = solver.solve(cnf);

if(result)
    return env->NewStringUTF("SATISFIABLE");
else
    return env->NewStringUTF("UNSATISFIABLE");
