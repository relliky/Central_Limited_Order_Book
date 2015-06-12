// My University number is: *** to be filled in ***
#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

// Online Reference for std::vector class  http://www.cplusplus.com/reference/vector/vector/
#include <vector>
#include <iostream>

#include <cstdio>
#include <cmath>

#include <iomanip> 
#include "CLOB_shared.hpp"
#include "AggregatedQuoteType.hpp"



class OrderBook
{
	public:
		
		OrderBook ();
		
		bool open (PriceType tickSizeSet, PriceType toleranceSet, std::ostream &log);
		
		bool close ();
		
		bool submit_order (OrderIdentifierType order_id, SellBuyType sb, PriceType price, VolumeType volume);
		
		bool cancel_order (OrderIdentifierType order_id);
		
		void print_order_info (std::ostream & where, OrderIdentifierType order_id) const;

		std::string format_price (PriceType price) const;
		
		std::vector<AggregatedQuoteType> get_aggregated_order_book(SellBuyType which_side) const;
				
	 private:
		// Status flags
		bool orderBookIsOpen;
		
		// variables for checking validity of orders
		PriceType tickSize;
		PriceType tolerance; 
		
		// sell and buy order queues in the order of priority
		std::vector<AggregatedQuoteType> sellOrderQueue;
		std::vector<AggregatedQuoteType> buyOrderQueue;

		// another copy of the order queue but in the order of time, i.e. TimeID
		std::vector<AggregatedQuoteType> 		    orderQueueAgainstTime;
		std::vector<AggregatedQuoteType>::size_type timeID;
		// Walking through queue orderQueueAgainstTime to find an timeID of an order for indexing by using its orderID.  
		std::vector<AggregatedQuoteType>::size_type getTimeIDByOrderID(OrderIdentifierType orderID) const;

		// an iostream pointer for logging purpose in this OrderBook
		std::ostream* logStreamPtr;

		//Execute transactions based the order with the highest priority, including market orders and limit orders
		void executeTransactions(void);
		
		// print out the information of the order just been submitted 
		void print_order_submitting_info (OrderIdentifierType order_id) const;

		// Compare prices of two orders
		CompareResultType compareTwoOrdersPrice(PriceType price1, PriceType price2);
		

	    //-------------------the following for debug usage only----------------------------------------
		// It prints sell or buy order queue in the order of priority 		
		void print_order_queue_info (SellBuyType typeOfOrderQueue) const;
		// It prints all orders in the order of time that they are submitted. 
		void printAllOrdersInfoAgainstTime (void) const;
	

		
		
};



#endif // ORDER_BOOK_HPP
