#include "visualise_helper.hpp"

void display_aggregate_order_book(const OrderBook & ob)
{
	// choose any one of (or indeed, any combination of) the following:
	
	//display_aggregate_order_book(ob, DisplayType::NONE);
	display_aggregate_order_book(ob, DisplayType::CONSOLE);
	//display_aggregate_order_book(ob, DisplayType::GRAPHICS);
}

