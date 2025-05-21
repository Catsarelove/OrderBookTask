# OrderBookTask
## dummy order book

input file containes a number of sequences (add, modify and delete order, subscribe, unsubscribe, print VWAP, print orderbook)


*LIST OF COMMANDS*
 * {noformat}ORDER ADD,<order id>,<symbol>,<side>,<quantity>,<price>{noformat}
Insert a new order into order book for a given instrument.

 * {noformat}ORDER MODIFY,<order id>,<quantity>,<price>{noformat}
Modify price and quantity in an already existing order.

 * {noformat}ORDER CANCEL,<order id>{noformat}
Withdraw an existing order.

 * {noformat}SUBSCRIBE BBO,<symbol>{noformat}
Starting from this line on, keep printing BBO information for a given instrument, unless it is filtered out by a program option (see SYNTAX)

 * {noformat}UNSUBSCRIBE BBO,<symbol>{noformat}
Starting from this line on, stop printing BBO information for a given instrument, unless there are subscribers left or unless the symbol is filtered out by a command line option (see SYNTAX)

 * {noformat}SUBSCRIBE VWAP,<symbol>,<quantity>{noformat}
Starting from this line on, keep printing VWAP information for a given instrument, unless it is filtered out by a program option (see SYNTAX)

 * {noformat}UNSUBSCRIBE VWAP,<symbol>,<quantity>{noformat}
Starting from this line on, stop printing VWAP information for a given instrument, unless there are subscribers left or unless the symbol is filtered out by a command line option (see SYNTAX)

 * {noformat}PRINT,<symbol>{noformat}
Print order book price levels for a given symbol in a form of
{noformat}
Bid                             Ask
<volume>@<price> | <volume>@<price>
{noformat}
where {{<volume>}} is a cumulative volume for all orders on a given price level. Bids must be sorted descending from top down, asks should be ascending from top down.

 * {noformat}PRINT_FULL,<symbol>{noformat}
Print all orders active in the order book, separated by their price levels.
