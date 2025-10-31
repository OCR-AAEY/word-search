#include <math.h>

#include "sigmoid.h"

double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

double sigmoid_derivative(double x)
{
    return exp(-x) / ((1.0 + exp(-x)) * (1.0 + exp(-x)));
}