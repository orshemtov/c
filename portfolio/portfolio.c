#include "portfolio.h"
#include "log.h"
#include "portfolio.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_portfolio(portfolio_t *portfolio, float initial_cash)
{
    log_debug("init_portfolio: called with initial_cash=%.2f", initial_cash);

    if (portfolio == NULL)
    {
        log_warn("init_portfolio: portfolio is NULL, returning");
        return;
    }

    log_debug("init_portfolio: initializing portfolio");
    portfolio->equity = initial_cash;
    portfolio->cash = initial_cash;
    portfolio->positions = NULL;
    portfolio->position_count = 0;

    log_info("init_portfolio: portfolio initialized - equity=%.2f, cash=%.2f, position_count=%d", portfolio->equity,
             portfolio->cash, portfolio->position_count);
}

void free_portfolio(portfolio_t *portfolio)
{
    log_debug("free_portfolio: called");

    if (portfolio == NULL)
    {
        log_warn("free_portfolio: portfolio is NULL, returning");
        return;
    }

    log_debug("free_portfolio: freeing positions");
    for (int i = 0; i < portfolio->position_count; i++)
    {
        position_t *position = &portfolio->positions[i];
        log_trace("free_portfolio: freeing orders for position %d with symbol=%s", i, position->underlying.symbol);
        free(position->orders);
    }
    free(portfolio->positions);

    portfolio->positions = NULL;
    portfolio->position_count = 0;
    portfolio->equity = 0.0;
    portfolio->cash = 0.0;

    log_info("free_portfolio: portfolio freed");
}

void create_order(portfolio_t *portfolio, order_t order)
{
    log_debug("create_order: called with symbol=%s, quantity=%d, price=%.2f", order.symbol, order.quantity,
              order.price);

    if (portfolio == NULL)
    {
        log_warn("create_order: portfolio is NULL, returning");
        return;
    }

    log_debug("create_order: current cash=%.2f", portfolio->cash);
    log_debug("create_order: current position_count=%d", portfolio->position_count);

    bool position_exists = false;
    for (int i = 0; i < portfolio->position_count; i++)
    {
        position_t *position = &portfolio->positions[i];
        log_trace("create_order: checking position %d with symbol=%s", i, position->underlying.symbol);

        if (strcmp(position->underlying.symbol, order.symbol) == 0)
        {
            log_debug("create_order: found existing position for %s", order.symbol);
            position_exists = true;

            log_debug("create_order: updating quantity from %d to %d", position->quantity,
                      position->quantity + order.quantity);
            position->quantity += order.quantity;

            // Add order to the position's order history
            position->orders = realloc(position->orders, (position->order_count + 1) * sizeof(order_t));
            position->orders[position->order_count] = order;
            position->order_count++;
            log_debug("create_order: added order to position, new order_count=%d", position->order_count);

            float order_cost = order.quantity * order.price;
            log_debug("create_order: order cost=%.2f, updating cash from %.2f to %.2f", order_cost, portfolio->cash,
                      portfolio->cash - order_cost);
            portfolio->cash -= order_cost;  // Negative quantity (sell) will add cash back
            break;
        }
    }

    if (!position_exists)
    {
        log_debug("create_order: creating new position for %s", order.symbol);

        portfolio->positions = realloc(portfolio->positions, (portfolio->position_count + 1) * sizeof(position_t));
        position_t *new_position = &portfolio->positions[portfolio->position_count];

        strncpy(new_position->underlying.symbol, order.symbol, 5);
        new_position->underlying.price = order.price;
        new_position->quantity = order.quantity;

        // Initialize order history
        new_position->orders = malloc(sizeof(order_t));
        new_position->orders[0] = order;
        new_position->order_count = 1;

        log_info("create_order: new position created - symbol=%s, quantity=%d, price=%.2f, order_count=%d",
                 new_position->underlying.symbol, new_position->quantity, new_position->underlying.price,
                 new_position->order_count);

        float order_cost = order.quantity * order.price;
        log_debug("create_order: order cost=%.2f, updating cash from %.2f to %.2f", order_cost, portfolio->cash,
                  portfolio->cash - order_cost);
        portfolio->cash -= order_cost;  // Negative quantity (sell) will add cash back

        portfolio->position_count++;
        log_debug("create_order: portfolio position_count increased to %d", portfolio->position_count);
    }

    log_debug("create_order: finished processing order");
    return;
}

double get_quote(const char *symbol)
{
    log_debug("get_quote: called with symbol=%s", symbol);
    
    if (symbol == NULL)
    {
        log_warn("get_quote: symbol is NULL, returning 0.0");
        return 0.0;
    }
    
    // Mock quotes for the symbols we use in main.c
    // In the future, this could use cURL to fetch real market data from APIs like Yahoo Finance, Alpha Vantage, etc.
    double quote = 0.0;
    
    if (strcmp(symbol, "VOO") == 0)
    {
        quote = 614.76;  // Vanguard S&P 500 ETF - current market price
    }
    else if (strcmp(symbol, "VGLT") == 0)
    {
        quote = 56.50;   // Vanguard Long-Term Treasury ETF - current market price
    }
    else if (strcmp(symbol, "VGIT") == 0)
    {
        quote = 60.06;   // Vanguard Intermediate-Term Treasury ETF - current market price
    }
    else if (strcmp(symbol, "GLD") == 0)
    {
        quote = 345.05;  // SPDR Gold Trust - current market price
    }
    else if (strcmp(symbol, "DBC") == 0)
    {
        quote = 22.26;   // Invesco DB Commodity Index - current market price
    }
    else
    {
        log_warn("get_quote: unknown symbol %s, returning default price of 100.00", symbol);
        quote = 100.00;  // Default fallback price
    }
    
    log_debug("get_quote: returning quote %.2f for symbol %s", quote, symbol);
    return quote;
}

void rebalance_portfolio(portfolio_t *portfolio, allocation_t *allocations, int num_allocations)
{
    log_debug("rebalance_portfolio: called with num_allocations=%d", num_allocations);

    if (portfolio == NULL || allocations == NULL || num_allocations <= 0)
    {
        log_warn("rebalance_portfolio: invalid parameters, returning");
        return;
    }

    log_info("rebalance_portfolio: current equity=%.2f, cash=%.2f", portfolio->equity, portfolio->cash);
    log_debug("rebalance_portfolio: current position_count=%d", portfolio->position_count);

    for (int i = 0; i < num_allocations; i++)
    {
        log_debug("rebalance_portfolio: processing allocation %d - symbol=%s, target_pct=%.2f", i,
                  allocations[i].symbol, allocations[i].target_pct);

        bool position_found = false;
        for (int j = 0; j < portfolio->position_count; j++)
        {
            position_t *position = &portfolio->positions[j];
            log_trace("rebalance_portfolio: checking position %d with symbol=%s", j, position->underlying.symbol);

            if (strcmp(position->underlying.symbol, allocations[i].symbol) == 0)
            {
                log_debug("rebalance_portfolio: found matching position for %s", allocations[i].symbol);
                position_found = true;

                float target_value = portfolio->equity * allocations[i].target_pct;
                float current_value = position->quantity * position->underlying.price;
                float difference = target_value - current_value;

                log_debug("rebalance_portfolio: target_value=%.2f, current_value=%.2f, difference=%.2f", target_value,
                          current_value, difference);

                if (difference > 0)
                {
                    // Buy more
                    int quantity_to_buy = (int)(difference / position->underlying.price);
                    log_info("rebalance_portfolio: need to buy %d shares of %s", quantity_to_buy, allocations[i].symbol);

                    order_t order = {.symbol = {0}, .quantity = quantity_to_buy, .price = position->underlying.price};
                    strncpy(order.symbol, position->underlying.symbol, 5);
                    create_order(portfolio, order);
                }
                else if (difference < 0)
                {
                    // Sell some
                    int quantity_to_sell = (int)(-difference / position->underlying.price);
                    if (quantity_to_sell > position->quantity)
                    {
                        log_warn("rebalance_portfolio: capping sell quantity from %d to %d (can't sell more than owned)",
                               quantity_to_sell, position->quantity);
                        quantity_to_sell = position->quantity; // Can't sell more than we have
                    }

                    log_info("rebalance_portfolio: need to sell %d shares of %s", quantity_to_sell, allocations[i].symbol);

                    order_t order = {.symbol = {0}, .quantity = -quantity_to_sell, .price = position->underlying.price};
                    strncpy(order.symbol, position->underlying.symbol, 5);
                    create_order(portfolio, order);
                }
                else
                {
                    log_debug("rebalance_portfolio: position %s is already balanced", allocations[i].symbol);
                }
                break;
            }
        }
        
        // If no existing position found, create a new position with target allocation
        if (!position_found)
        {
            log_debug("rebalance_portfolio: no existing position for %s, creating new position", allocations[i].symbol);
            
            float target_value = portfolio->equity * allocations[i].target_pct;
            // Get current market price for the symbol
            double current_price = get_quote(allocations[i].symbol);
            int quantity_to_buy = (int)(target_value / current_price);
            
            if (quantity_to_buy > 0)
            {
                log_info("rebalance_portfolio: creating new position - buying %d shares of %s at $%.2f", 
                         quantity_to_buy, allocations[i].symbol, current_price);
                
                order_t order = {.symbol = {0}, .quantity = quantity_to_buy, .price = current_price};
                strncpy(order.symbol, allocations[i].symbol, 5);
                create_order(portfolio, order);
            }
        }
    }

    log_info("rebalance_portfolio: finished rebalancing");
}