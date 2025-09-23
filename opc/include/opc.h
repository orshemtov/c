#ifndef OPC_H
#define OPC_H

#include <stdbool.h>

#define RISK_FREE_RATE 0.05

typedef struct
{
    double price;
    double volatility;
} underlying_t;

typedef struct
{
    double strike;
    char *expiration_date;
    bool is_call;
} option_t;

// Function to calculate the cumulative distribution function for a standard normal distribution
static double normal_cdf(double x);

// Convert from expiration date to time in years
double expiration_to_time(const char *expiration_date);

// Function to calculate the Black-Scholes option price
// S: Current stock price
// K: Option strike price
// T: Time to expiration in years
// r: Risk-free interest rate
// sigma: Volatility of the underlying stock
double black_scholes(double S, double K, double T, double r, double sigma, bool is_call);

#endif