// My University number is: *** to be filled in ***
#include <cstdlib>
#include <iostream>

#include "CLOB_shared.hpp"
#include "AggregatedQuoteType.hpp"
#include "OrderBook.hpp"
#include "visualise_helper.hpp"

void CLOB_simple_test()
{
	OrderBook book;
	display_aggregate_order_book(book);
	
	book.open(0.1, 0.01, std::cout); // return values ignored in this example
	display_aggregate_order_book(book);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block A - there should be no matches yet" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.submit_order(1u, SellBuyType::SELL, 10.1, 100u);
	display_aggregate_order_book(book);
	book.submit_order(2u, SellBuyType::SELL, 10.3, 75u);
	display_aggregate_order_book(book);
	book.submit_order(3u, SellBuyType::BUY, 9.8, 200u);
	display_aggregate_order_book(book);
	book.submit_order(4u, SellBuyType::BUY, 10.0, 150u);
	display_aggregate_order_book(book);
	book.submit_order(5u, SellBuyType::BUY, 9.9, 100u);
	display_aggregate_order_book(book);
	book.submit_order(6u, SellBuyType::SELL, 10.1, 50u); // this entry shares the same price as a previous entry
	display_aggregate_order_book(book);
	book.print_order_info(std::cout, 1u);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block B - there should be a match" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.submit_order(7u, SellBuyType::BUY, 10.2, 200u);
	book.print_order_info(std::cout, 1u);
	book.print_order_info(std::cout, 7u);
	display_aggregate_order_book(book);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block C - market orders" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.submit_order(10u, SellBuyType::SELL, 0.0, 200u);
	book.print_order_info(std::cout, 10u);
	display_aggregate_order_book(book);
	book.submit_order(11u, SellBuyType::BUY, 0.0, 100u);
	book.print_order_info(std::cout, 11u);
	display_aggregate_order_book(book);
	book.submit_order(12u, SellBuyType::BUY, 0.0, 100u);
	book.print_order_info(std::cout, 12u);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block D - manipulating existing orders" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.cancel_order(5u);
	book.print_order_info(std::cout, 5u);
	display_aggregate_order_book(book);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block E - invalid orders/actions" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.cancel_order(5u);
	book.cancel_order(2u);
	book.cancel_order(50u);
	book.print_order_info(std::cout, 50u);
	book.submit_order(15u, SellBuyType::SELL, 10.15, 50u);
	
	std::cerr << "*********************" << std::endl;
	std::cerr << "Block F - end of the first day's trading" << std::endl;
	std::cerr << "*********************" << std::endl;
	book.close();
	book.print_order_info(std::cout, 3u);
	display_aggregate_order_book(book);

	std::cerr << "*********************" << std::endl;
	std::cerr << "End" << std::endl;
	std::cerr << "*********************" << std::endl;
}

int main()
{
	CLOB_simple_test();
	return EXIT_SUCCESS;
}

