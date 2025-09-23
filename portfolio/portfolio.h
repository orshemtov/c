#ifndef PORTFOLIO_H
#define PORTFOLIO_H

typedef struct
{
    char symbol[5];
    double price;
} underlying_t;

typedef struct
{
    char symbol[5];
    int quantity;
    double price;
} order_t;

typedef struct
{
    underlying_t underlying;
    int quantity;
    order_t *orders; // We can do this another way?
    int order_count; // Track number of orders
} position_t;

typedef struct
{
    float equity;
    float cash;
    position_t *positions; // We can do this another way?
    int position_count;    // Track number of positions
} portfolio_t;

typedef struct
{
    char symbol[5];
    float target_pct;
} allocation_t;

// Some functions here can return error codes?
// Not just void

// Should we use const anywhere around here?

void init_portfolio(portfolio_t *portfolio, float initial_cash);
void free_portfolio(portfolio_t *portfolio);

void create_order(portfolio_t *portfolio, order_t order);

// Get current market quote for a symbol
// In the future, this could use cURL to fetch real market data
double get_quote(const char *symbol);

// Allocations is passed in as a pointer to the first element of an array
// Is there way to do it by value? like a Python list? or a map?
// Do we need num_allocations if we can use sizeof()?
void rebalance_portfolio(portfolio_t *portfolio, allocation_t *allocations, int num_allocations);

#endif /* PORTFOLIO_H */