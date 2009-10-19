
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib>

#include "logreg.h"

LogReg::LogReg(int variables, int learning_rate)
{
    this->vars = variables;
    this->t = new double[this->vars + 1];
    this->b = new double[this->vars + 1];
    
    memset(this->t, 0, sizeof(double) * (this->vars + 1));
    memset(this->b, 0, sizeof(double) * (this->vars + 1));
    
    this->rate = learning_rate;
    
    //
    this->pushups = 0;
    this->fail = 0;
}

LogReg::~LogReg()
{
    delete[] this->t;
    delete[] this->b;
}

void LogReg::set(int i, double v)
{
    this->t[i] = v;
}

double LogReg::get(int i)
{
    return this->t[i];
}

double LogReg::z(double *input)
{
    double z = 0.0;
    for (int i = 0; i < this->vars; i++)      // LOOPY!
        z += this->t[i] * input[i];           // Sum this 0j * xj
    return z;
}

double LogReg::p(double z)
{
    return 1.00 / (1.00 + exp(-z))
}

double LogReg::g(double z, bool truth)
{
    return (truth ? 1 : 0) - this->p(z);
}


bool LogReg::predict(double *input)
{
    double z = this->z(input);
    return this->p(z) > 0.5;
}

bool LogReg::train(double *input, bool truth)
{
    bool predict = this->predict(input);
    this->pushup++;

// for each epoch ???    
// FLAW: only training on one input for this go.... look at lr.c:161-169 //TODO may fail...

    memset(this->b, 0, sizeof(double) * (this->vars + 1));
    
    double z = this->z(input);
    double g = this->g(z, truth);
    
    for (int j = 0; j <= this->vars; j++)
    {
        double xj = (j < this->vars) ? input[j] : 1; // bias. 
        this->b[j] += xj * g;
    }
    
    for (int j = 0; j < this->vars; j++)
        this->t[j] += this->rate * this->b[j];
    
// endfor epoch ???
    
    if (predict == truth) { this->win++; return true; }
    this->fail++; return false;
}
