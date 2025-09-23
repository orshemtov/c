#include "opc.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static double normal_cdf(double x)
{
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

// We assume expiration date is in the format "YYYY-MM-DD"
double expiration_to_time(const char *expiration_date)
{
    // Parse the expiration date
    int exp_year, exp_month, exp_day;
    if (sscanf(expiration_date, "%d-%d-%d", &exp_year, &exp_month, &exp_day) != 3)
    {
        return -1.0; // Invalid date format
    }

    // Get current time
    time_t now = time(NULL);
    struct tm *current_tm = localtime(&now);

    // Create tm structure for expiration date
    struct tm exp_tm = {0};
    exp_tm.tm_year = exp_year - 1900; // tm_year is years since 1900
    exp_tm.tm_mon = exp_month - 1;    // tm_mon is 0-11
    exp_tm.tm_mday = exp_day;
    exp_tm.tm_hour = 16; // Options typically expire at 4 PM EST
    exp_tm.tm_min = 0;
    exp_tm.tm_sec = 0;
    exp_tm.tm_isdst = -1; // Let system determine DST

    // Convert to time_t
    time_t exp_time = mktime(&exp_tm);
    if (exp_time == -1)
    {
        return -1.0; // Invalid date
    }

    // Calculate difference in seconds
    double diff_seconds = difftime(exp_time, now);

    // Convert to years (365.25 days per year to account for leap years)
    double years = diff_seconds / (365.25 * 24 * 3600);

    return years > 0 ? years : 0.0; // Return 0 if expiration has passed
}

double black_scholes(double S, double K, double T, double r, double sigma, bool is_call)
{
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    if (is_call)
    {
        return S * normal_cdf(d1) - K * exp(-r * T) * normal_cdf(d2);
    }
    else
    {
        return K * exp(-r * T) * normal_cdf(-d2) - S * normal_cdf(-d1);
    }
}
