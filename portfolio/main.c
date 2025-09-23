#include "portfolio.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    portfolio_t portfolio;
    init_portfolio(&portfolio, 10000.0);

    allocation_t allocations[] = {
        {
            "VOO",
            0.30,
        },
        {
            "VGLT",
            0.40,
        },
        {
            "VGIT",
            0.15,
        },
        {
            "GLD",
            0.075,
        },
        {
            "DBC",
            0.075,
        },
    };

    rebalance_portfolio(&portfolio, allocations, 5);

    // Print the position symbol, quantity, price, dollar value and percentage of portfolio
    printf("Portfolio Positions:\n");
    printf("Symbol | Quantity | Price  | Value   | %% of Portfolio\n");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < portfolio.position_count; i++)
    {
        position_t *position = &portfolio.positions[i];
        float position_value = position->quantity * position->underlying.price;
        float position_pct = position_value / portfolio.equity * 100.0;
        printf("%-6s | %8d | %6.2f | %7.2f | %6.2f%%\n", position->underlying.symbol, position->quantity,
               position->underlying.price, position_value, position_pct);
    }
    printf("-----------------------------------------------------\n");
    printf("Total Equity: %.2f\n", portfolio.equity);
    printf("Total Cash:   %.2f\n", portfolio.cash);
    printf("Total Value:  %.2f\n", portfolio.equity + portfolio.cash);

    free_portfolio(&portfolio);

    return EXIT_SUCCESS;
}