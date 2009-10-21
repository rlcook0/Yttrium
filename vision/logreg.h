#pragma once

#include <stdio.h>
#include <string.h>


class Trainer {
public:
    double *values;
    bool truth;
};

class LogReg {
    
public:
    
    LogReg(int variables, double learning_rate);
    ~LogReg();
    
    // For loading from file.
    void set(int i, double v);
    
    // For saving to a file.
    double get(int i);
    
    bool predict(double *input);
    void add_to_batch(double *input, bool truth);
    void add_to_batch(Trainer &t);
    
    // Process a single batch. Returns the total amount changed
    // from the weights.
    double gradient_decent(); // process batch.
    
private:
    
    // computes x for a set of theta's and x's
    // x = SUM {for j in vars} ( bj * xj )
    double z(double *input);
    
    // computes p(z) = 1 / (1 + exp(-z))
    double logistic(double z);
    
    // computes g(z) = truth - p(z)
    double objective_function(double *input, bool truth);
    
    int num_variables;
    int num_examples;
    
    double *coeff; // Thetas
    double *batch; // Thetas per round.
    double learning_rate; // learning rate
    
    int pushup;
    int fail;
    int win;
    
    int tp, tn, fp, fn;
};

