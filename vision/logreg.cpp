
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "logreg.h"

using namespace std;

LogReg::LogReg(int variables, double learning_rate)
{
    this->num_variables = variables;
    this->coeff = new double[this->num_variables + 1];
    this->batch = new double[this->num_variables + 1];
    
    memset(this->coeff, 0, sizeof(double) * (this->num_variables + 1));
    memset(this->batch, 0, sizeof(double) * (this->num_variables + 1));
    
    this->learning_rate = learning_rate;
    
    this->num_examples = 0;
    this->fail = 0;
    
    tp = tn = fp = fn = 0;
}

LogReg::~LogReg()
{
    delete[] this->coeff;
    delete[] this->batch;
}

void LogReg::set(int i, double v)
{
    this->coeff[i] = v;
}

double LogReg::get(int i)
{
    return this->coeff[i];
}

double LogReg::z(double *input)
{
    double z = 0.0;
    for (int i = 0; i < this->num_variables; i++)      // LOOPY!
        z += this->coeff[i] * input[i];           // Sum this 0j * xj
    return z;
}

double LogReg::logistic(double z)
{
    return 1.00 / (1.00 + exp(-z));
}

double LogReg::objective_function(double *input, bool truth)
{
    double z = this->z(input);
    return (truth ? 1 : 0) - this->logistic(z);
}


bool LogReg::predict(double *input)
{
    double z = this->z(input);
    return this->logistic(z) > 0.5;
}

void LogReg::add_to_batch(double *input, bool truth)
{
    this->num_examples++;
    
    bool is_class = this->predict(input);
    double objective_value = this->objective_function(input, truth);
    
    for (int j = 0; j <= this->num_variables; j++)
    {
        double xj = (j < this->num_variables) ? input[j] : 1; // bias. 
        this->batch[j] += xj * objective_value;
    }
    
    if (is_class && truth) {
        tp++;
    } else if (is_class && !truth) {
        fp++;
    } else if (!is_class && truth) {
        fn++;
    } else if (!is_class && !truth) {
        tn++;
    }
}

void LogReg::add_to_batch(Trainer &t)
{
    this->add_to_batch(t.values, t.truth);
}

double LogReg::gradient_decent()
{
    double total = 0;
    for (int j = 0; j <= this->num_variables; j++) {
        double diff = this->learning_rate * this->batch[j];
        total += diff;
        this->coeff[j] += diff;
    }
    cout << "tp: " << tp << " fp: " << fp << " fn: " << fn << " tn: " << tn << " totaldiff: " << total << endl;
    tp = fp = fn = tn = 0;
    
    memset(this->batch, 0, sizeof(double) * (this->num_variables + 1));
    
    return total;
}
