#pragma once


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
    bool train(double *input, bool truth);
    bool train(Trainer &t);
    void learn(); // process batch.
    
private:
    
    // computes x for a set of theta's and x's
    // x = SUM {for j in vars} ( bj * xj )
    double z(double *input);
    
    // computes p(z) = 1 / (1 + exp(-z))
    double p(double z);
    
    // computes g(z) = truth - p(z)
    double g(double z, bool truth);
    
    int vars;
    
    double *t; // Thetas
    double *b; // Thetas per round.
    
    double rate; // learning rate

    
    int pushup;
    int fail;
    int win;
};

