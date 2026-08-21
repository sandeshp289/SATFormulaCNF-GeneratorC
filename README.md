# SATFormulaCNF-GeneratorC

Description:

    SAT Formula (CNF-Conjuctive Normal Form) Generator in C/C++

Input Files:

    CNF_combo.c                    : C/C++ code (for combinational circuits)

    CNF_seq.c                      : C/C++ code (for sequential circuits)

    c17.lev                        : Input circuit file (for e.g. combinational circuit c17) in lev format
    
    s27.lev                        : Input circuit file (for e.g. sequential circuit s27) in lev format

Output Files:

    c17.cnf                          : Output CNF file (for e.g. c17)

    s27.cnf                          : Output CNF file (for e.g. s27)
    
Compilation:

    g++ CNF_combo.c
    
    g++ CNF_seq.c

Execution:

    ./a.out c17

    ./a.out s27
