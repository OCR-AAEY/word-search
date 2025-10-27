#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

typedef struct Neural_Network Neural_Network;

struct Training_Data
{
    Matrix *input;
    Matrix *expected;
};
typedef struct Training_Data Training_Data;

#endif
