#include "opc.h"
#include <math.h>
#include <stdio.h>

int main()
{
    printf("Black-Scholes Option Pricing Calculator\n");
    printf("=======================================\n\n");

    // Test 1: Basic test with current implementation
    printf("Test 1: Basic Option Pricing\n");
    printf("-----------------------------\n");

    underlying_t underlying = {100.0, 0.2};
    option_t call_option = {105.0, "2026-01-17", true}; // Updated to future date
    option_t put_option = {105.0, "2026-01-17", false}; // Put option with same parameters

    double T = expiration_to_time(call_option.expiration_date);
    if (T < 0)
    {
        fprintf(stderr, "Invalid expiration date\n");
        return 1;
    }

    printf("Stock Price: $%.2f\n", underlying.price);
    printf("Strike Price: $%.2f\n", call_option.strike);
    printf("Volatility: %.1f%%\n", underlying.volatility * 100);
    printf("Risk-free Rate: %.1f%%\n", RISK_FREE_RATE * 100);
    printf("Time to Expiration: %.4f years\n", T);
    printf("Expiration Date: %s\n\n", call_option.expiration_date);

    double call_price = black_scholes(underlying.price, call_option.strike, T, RISK_FREE_RATE, underlying.volatility, call_option.is_call);
    double put_price = black_scholes(underlying.price, put_option.strike, T, RISK_FREE_RATE, underlying.volatility, put_option.is_call);

    printf("Call Option Price: $%.4f\n", call_price);
    printf("Put Option Price: $%.4f\n", put_price);

    // Test 2: Put-Call Parity validation
    printf("\nTest 2: Put-Call Parity Validation\n");
    printf("-----------------------------------\n");
    double parity_left = call_price - put_price;
    double parity_right = underlying.price - call_option.strike * exp(-RISK_FREE_RATE * T);
    printf("C - P = %.6f\n", parity_left);
    printf("S - K*e^(-rT) = %.6f\n", parity_right);
    printf("Difference: %.8f (should be close to 0)\n", fabs(parity_left - parity_right));

    if (fabs(parity_left - parity_right) < 1e-6)
    {
        printf("✓ Put-Call Parity holds!\n");
    }
    else
    {
        printf("✗ Put-Call Parity failed!\n");
    }

    // Test 3: Different scenarios
    printf("\nTest 3: Different Market Scenarios\n");
    printf("-----------------------------------\n");

    // At-the-money option
    option_t atm_call = {100.0, "2026-01-17", true};
    double atm_price = black_scholes(underlying.price, atm_call.strike, T, RISK_FREE_RATE, underlying.volatility, atm_call.is_call);
    printf("At-the-money call (K=$%.0f): $%.4f\n", atm_call.strike, atm_price);

    // Deep in-the-money option
    option_t itm_call = {80.0, "2026-01-17", true};
    double itm_price = black_scholes(underlying.price, itm_call.strike, T, RISK_FREE_RATE, underlying.volatility, itm_call.is_call);
    printf("In-the-money call (K=$%.0f): $%.4f\n", itm_call.strike, itm_price);

    // Deep out-of-the-money option
    option_t otm_call = {120.0, "2026-01-17", true};
    double otm_price = black_scholes(underlying.price, otm_call.strike, T, RISK_FREE_RATE, underlying.volatility, otm_call.is_call);
    printf("Out-of-the-money call (K=$%.0f): $%.4f\n", otm_call.strike, otm_price);

    // Test 4: High volatility impact
    printf("\nTest 4: Volatility Impact\n");
    printf("-------------------------\n");
    underlying_t high_vol = {100.0, 0.5}; // 50% volatility
    double high_vol_price = black_scholes(high_vol.price, call_option.strike, T, RISK_FREE_RATE, high_vol.volatility, call_option.is_call);
    printf("Low volatility (20%%): $%.4f\n", call_price);
    printf("High volatility (50%%): $%.4f\n", high_vol_price);
    printf("Volatility premium: $%.4f\n", high_vol_price - call_price);

    return 0;
}