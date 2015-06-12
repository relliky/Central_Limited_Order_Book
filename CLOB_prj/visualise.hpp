#ifndef VISUALISE_HPP
#define VISUALISE_HPP

enum class DisplayType{CONSOLE,GRAPHICS,NONE,TEX};
class OrderBook;

void display_aggregate_order_book(const OrderBook & ob, DisplayType dt);

#endif

