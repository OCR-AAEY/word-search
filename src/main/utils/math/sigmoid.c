#include <math.h>

#include "sigmoid.h"

float sigmoid(float x) { return 1.0f / (1.0f + exp(-x)); }

float sigmoid_derivative(float x)
{
    return exp(-x) / ((1.0f + exp(-x)) * (1.0f + exp(-x)));
}