// My University number is: *** to be filled in ***
#include "OrderBook.hpp"

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- Public Methods-------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

OrderBook::OrderBook ()
{
	orderBookIsOpen = false;
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Open and Close the OrderBook database
 *  Description:
 *  Reject all orders which are  submitted before the  order book is open or after  it is closed
 *  (though note that  it may re-open again later  for the next trading day).
 *  
 *  At close,  deactivate the  remaining parts of any  orders that  remain active  in the  order book
 *  (but note that  it should still be possible to inspect the  state of all orders). Specially,
 *  unexecuted orders should be considered as expired and orders that  were  partially executed before 
 *  the order book was closed should now be truncated and  considered as filled.
 *  
 *  If open or close are executed when  the order book is already open or closed,  or invalid arguments 
 *  are  supplied to  open, false should be  returned (no  message should be printed to std::cout, but 
 *  you may print  an appropriate message to std::cerr).
 *  
 *  tick_size speci1es the  tick size:  all prices  of submitted orders should be a multiple of the  
 *  tick size,  or within  the  appropriate tolerance of such  a multiple.  Speci1cally,  if an  order 
 *  is submitted with  the  price  p and  |p ? N tick_size| >  tolerance for all natural numbers N, 
 *  such  an order should be rejected because the  price  is invalid.  On the other hand, if there exists 
 *  an N such that  |p ? N tick_size| ¡Ü tolerance then the price p can be accepted (it is approximately 
 *  a multiple of tick_size). In such case, you may choose to either store p or store N tick_size. 
 *  Hint:  you cannot use  the  integer remainder operator with  2oating point  types.   
 *  Additionally,  the  foating point  version fmod does  not work well.
 *  
 *  The order book should treat prices  which differ by ¡Ü 2*tolerance as being equal. 
 *  This applies for both the price-time priority principle and the get_aggregated_order_book function.
 *  
 *  The tolerance can always be assumed to be less than or equal to tick_size/10. 
 *  Note that  the tolerance and  tick_size may change on the next call to open.
 *  
 *  log is an open stream where the  order book should write messages about submitted/- cancelled orders 
 *  and  executed trades (transactions). In the  event that  the  order book needs to  write  a message 
 *  before the  1rst  call to  open, log should default to  std:: cerr.  Hint:  you  will need to  save
 *  a pointer to  log as  a member variable within  the class.
 *  Each trade message should contain the  identi1ers (OrderIdentifierType) of orders involved  in the  
 *  trade, the  price  and  the  traded volume,  formatted as per  the  examples later.
 *  Whenever outputting prices,  format them through the format_price function 1rst.
 *  If the  order book  successfully opens/closes, the  messages "Order  book  open." or
 *  "Order  book  closed." should respectively be printed to log. 
 */
bool OrderBook::open (PriceType tickSizeSet, PriceType toleranceSet, std::ostream &log)
{
	if(orderBookIsOpen ==  false)
	{
		tickSize     = tickSizeSet;
		tolerance    = toleranceSet*1.01; // leave a little bit margin for simple comparison between float point numbers
		logStreamPtr = &log;
		timeID       = 0;
		
		orderBookIsOpen = true;
		(*logStreamPtr) << "Order book open." << std::endl;
		return true;		
	}else{
		std::cerr << "Order book has already been open" << std::endl;
		return false;	
	}
	
}

/** Close the orderbook and deactivate the remaining orders
 *  Description:
 *  At close,  deactivate the  remaining parts of any  orders that  remain active  in the  order book
 *  (but note that  it should still be possible to inspect the  state of all orders). Specially,
 *  unexecuted orders should be considered as expired and orders that  were  partially executed before 
 *  the order book was closed should now be truncated and  considered as filled.
 */
bool OrderBook::close ()
{
	std::vector<AggregatedQuoteType>::size_type index = 0;
	
	if(orderBookIsOpen ==  true)
	{
		(*logStreamPtr) << "Order book closed." << std::endl;
		orderBookIsOpen = false;
		
		// remove all elements from active queue
		sellOrderQueue.clear();
		buyOrderQueue.clear();
		// Walking through order list and deactivate any active status
		for(index = 0; index < orderQueueAgainstTime.size(); index++)
		{
			if(orderQueueAgainstTime.at(index).get_executed_volume() > 0)
			{
				orderQueueAgainstTime.at(index).closePartialFilledMarketOrder();
				orderQueueAgainstTime.at(index).changeStatusTo(FILLED);
			}else{
				if(orderQueueAgainstTime.at(index).get_status() == ACTIVE)
				{
					orderQueueAgainstTime.at(index).changeStatusTo(EXPIRED);									
				}
			}
		}
				
		
		return true;
	}
	
	std::cerr << "Order book has already been closed" << std::endl;
	return false;
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Submit an order into the OrderBook
 *  Description:
 *  Check if the order can be accepted; recall the conditions for acceptance in terms of the
 *  tick_size etc, and also that order volumes must be strictly positive. A price of zero
 *  indicates that the order is to be a market or expire order. Check also that the identi1er
 *  supplied is unique (you do not need to worry about generating such unique identi1ers,
 *  these are hardcoded into the CLOB_simple_test function). Note that order identi1ers
 *  need not be sequential.
 *  If the order is accepted, execute it if possible according to the price-time priority, otherwise
 *  add it to the order book for potential execution later. If the order is accepted
 *  but is never added to the active order book (because it is immediately fully executed),
 *  information about the order still needs to be retained so that it can be queried later by
 *  print_order_info.
 *  Return true if the order was accepted, or false otherwise.
 *  Write the order message to log. Write the trade messages to log if the order was (at
 *  least partially) executed.
 *  Once an order has been submitted, it cannot be changed (but it can be cancelled completely
 *  ¡ª clients should therefore place a new order to effect a change, but in doing so
 *  will lose their place in the queue).
 */
bool OrderBook::submit_order (OrderIdentifierType order_id, SellBuyType sb, PriceType price, VolumeType volume)
{
	// a copy of the new order, which will be popped into order queues later
	AggregatedQuoteType newOrder(order_id, sb, price, volume);
	// iterator of order queues
	std::vector<AggregatedQuoteType>::iterator itr;
	// various flags
	bool twoPricesAreEqual = false;	
	// temporary variables
	PriceType priceBuf = price;
	
	// Check if the order can be accepted; recall the conditions for acceptance in terms of the
 	// tick_size etc, and also that order volumes must be strictly positive.
	// volume is an unsigned type specified in the spec, therefore there is no point of comparing volume with 0.
	if(price < 0)
	{
		(*logStreamPtr) << "Order not accepted." << std::endl;
	}
	// online manual for fasb() can be found in      http://www.cplusplus.com/reference/cmath/fabs/
	while(fabs(priceBuf) > tickSize)
	{
		priceBuf-= tickSize;
	}
	if(fabs(priceBuf) > tolerance)
	{
		(*logStreamPtr) << "Order not accepted." << std::endl;
		return false;
	}	
	
	
	// Make a copy to orderQueueAgainstTime
	// This is another copy of the order queue but in the order of time, i.e. orderID
	// N.B. any methods related to this queue rely on that user will input order starting from 1 and increment by 1 each time
	// This queue is only for displaying information.
	orderQueueAgainstTime.push_back(newOrder);
	timeID++;
	
	// print submit info
	this->print_order_submitting_info(order_id);
	
	// Make a copy to sell or buy order queues in the order of its priority
	if(sb == SellBuyType::SELL)
	{  
		itr = sellOrderQueue.begin();
		if(sellOrderQueue.size() > 0) 
		{
			// 1. Market order
			// If it is a market order, just put it at the top priority
			if(newOrder.get_price() == 0)
			{
				sellOrderQueue.insert(sellOrderQueue.begin(), newOrder);
				goto EXECUTE_NEW_ORDER;				
			}
			
			// 2. Limit order
			// If the queue is not empty, put the new order into this queue based on its priority.
			// The order with the highest priority locates at the beginning of this queue. 
			// The order with the lowest priority located at the end of this queue. 
			
			// e.g. assuming there is a random number to be inserted into a sorted queue (10, 8, 5, 5, 5, 3) in a order;
			// Walking through the queue till the iterator points to an order with price lower than this new order.
			while((this->compareTwoOrdersPrice(price, (*itr).get_price())) == GREATER)
			{
				itr++;
				// e.g. case1: if number 1 inserted into (3,5,5,5,8,10);
				// Make sure the if the iterator reaches the end of the queue, this new order will be inserted and no futher
				// comparison will be procressed. Otherwise, there will be run-time error of deferencing this iterator.
				if(itr == sellOrderQueue.end())
				{
					sellOrderQueue.insert(itr, newOrder);
					goto EXECUTE_NEW_ORDER;
				}
			}
			
			// e.g. case2: if number 5 inserted into (3,5,5,5,8,10)
			while(this->compareTwoOrdersPrice(price, (*itr).get_price()) == EQUAL)
			{
				twoPricesAreEqual = true;
				itr++;
			}
			if(twoPricesAreEqual == true){
				// put the newOrder in the end of the orders with same price
				// as it implied it is the latest order and therefore it has 
				// least priority within the orders with same price for sellOrderQueue
				sellOrderQueue.insert(itr, newOrder);
				goto EXECUTE_NEW_ORDER;	
			}
			
			// e.g. case3: if number 4 inserted into (3,5,5,5,8,10)
			if(this->compareTwoOrdersPrice(price, (*itr).get_price()) == LESS)
			{
				sellOrderQueue.insert(itr, newOrder);
				goto EXECUTE_NEW_ORDER;	
			}					
		}else{
				// case4: if the queue is empty, add the new order as the first order into this queue. 
				sellOrderQueue.push_back(newOrder);
			 }
	} // end of if(sb == SellBuyType::SELL)

	if(sb == SellBuyType::BUY)
	{
		itr = buyOrderQueue.begin();
		
		if(buyOrderQueue.size() > 0) 
		{
			// 1. Market order
			// If it is a market order, just put it at the top priority
			if(newOrder.get_price() == 0)
			{
				buyOrderQueue.insert(buyOrderQueue.begin(), newOrder);
				goto EXECUTE_NEW_ORDER;
			}
			
			// 2. Limit order
			// If the queue is not empty, put the new order into this queue based on its priority.
			// The order with the highest priority locates at the beginning of this queue. 
			// The order with the lowest priority located at the end of this queue. 

			// e.g. assuming there is a random number to be inserted into a sorted queue (3,5,5,5,8,10) in a order;
			// Walking through the queue till the iterator points to an order with price higher than this new order.
			while((this->compareTwoOrdersPrice(price, (*itr).get_price()))==LESS)
			{
				itr++;
				// e.g. case1: if number 11 inserted into (3,5,5,5,8,10);
				// Make sure the if the iterator reaches the end of the queue, this new order will be inserted and no futher
				// comparison will be procressed. Otherwise, there will be run-time error of deferencing this iterator.
				if(itr == buyOrderQueue.end())
				{
					buyOrderQueue.insert(itr, newOrder);
					goto EXECUTE_NEW_ORDER;
				}
			}
			
			// e.g. case2: if number 5 inserted into (3,5,5,5,8,10)
			while(this->compareTwoOrdersPrice(price, (*itr).get_price()) == EQUAL)
			{
				twoPricesAreEqual = true;
				itr++;
			}
			if(twoPricesAreEqual == true){
				// put the newOrder in the end of the orders with same price
				// as it implied it is the latest order and therefore it has 
				// least priority within the orders with same price for buyOrderQueue
				buyOrderQueue.insert(itr, newOrder);
				goto EXECUTE_NEW_ORDER;	
			}
			
			// e.g. case3: if number 4 inserted into (3,5,5,5,8,10)
			if(this->compareTwoOrdersPrice(price, (*itr).get_price()) == GREATER)
			{
				buyOrderQueue.insert(itr, newOrder);
				goto EXECUTE_NEW_ORDER;	
			}					
		}else{
				// case4: if the queue is empty, add the new order as the first order into this queue. 
				buyOrderQueue.push_back(newOrder);
				goto EXECUTE_NEW_ORDER;	
			 }
		
	} // end of if(sb == SellBuyType::BUY)

	// If the new order is immediately executable, the transaction will be executed.
	EXECUTE_NEW_ORDER:{
						this->executeTransactions();
						return true;
					  }
	return true;
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Remove the order from the active order book
 *  Description:
 *  Remove the order from the active order book (but again note that information concerning
 *  the order must still be retained). Reject the cancellation and return false if the
 *  order has already been cancelled, has already expired, is already fully executed, or the
 *  order book is closed. Also, return false if the order is not found. Otherwise, return true.
 *  
 *  Write the order message to log.
 *  If the order is already partially executed, the total volume should be reduced to the
 *  already executed volume, and the order should be considered 1lled instead of cancelled.
 *  (The order message need not re2ect the change in status to 1lled in this instance.)
 */
bool OrderBook::cancel_order (OrderIdentifierType order_id)
{
	std::vector<AggregatedQuoteType>::size_type index = 0;
	bool orderIsFound = false;
	OrderStatusType  localOrderStatus;
	
	// check if the orderID is valid	
	for(index = 0; index < orderQueueAgainstTime.size(); index++)
	{
		if(orderQueueAgainstTime.at(index).get_number_of_orders() == order_id) 
			orderIsFound = true;
	}
	
	if(orderIsFound == false)
	{
		(*logStreamPtr) << "Order "<< order_id <<" not found." << std::endl;
		return false;
	}
	
	// check if the orderbook is open
	if(orderBookIsOpen == false)
	{
		(*logStreamPtr) << "Order " << order_id << " cannot be cancelled." << std::endl;
		return false;		
	} 
	
	// check if the order is active when the orderbook is open
	localOrderStatus = orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).get_status();
	if(localOrderStatus != ACTIVE)
	{
		(*logStreamPtr) << "Order " << order_id << " cannot be cancelled." << std::endl;
		return false;
	}
	
	// If the order is active. Cancel it and properly record it.
	// If the order is already partially executed, the total volume should be reduced to the
    // already executed volume and the order should be considered fileed instead of cancelled.
	orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).changeStatusTo(CANCELLED);
	if(orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).get_executed_volume() > 0)
	{
		orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).cancelPartialFilledMarketOrder();
		orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).changeStatusTo(FILLED);	
	}
	
	// remove the active order from the queue
	std::vector<AggregatedQuoteType>::iterator itr;
	
	if(orderQueueAgainstTime.at(this->getTimeIDByOrderID(order_id)).get_typeOfOrder() == SellBuyType::SELL)
	{
		for(itr = sellOrderQueue.begin(); itr < sellOrderQueue.end(); itr++)
		{
			if( (*itr).get_number_of_orders() == order_id) sellOrderQueue.erase(itr);
		}
	}else{
			for(itr = buyOrderQueue.begin(); itr < buyOrderQueue.end(); itr++)
			{
				if( (*itr).get_number_of_orders() == order_id) buyOrderQueue.erase(itr);
			}		
		 }
		 
	(*logStreamPtr) << "Order " << order_id << " cancelled." << std::endl;			 
	return true;
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Display the information about the order corresponding to the given order identi1er on the given output stream.
 *  Description:
 *  The information should include:
 *  - the order status,
 *     * ACTIVE, if the order is in the order book and not fully executed;
 *     * FILLED, if the order was (essentially) fully executed;
 *     * CANCELLED, if the order was cancelled;
 *     * EXPIRED, if the order was submitted on a previous trading day.
 *  - The type of the order: whether it is a sell or buy order;
 *  - The current order price;
 *  - The total order volume;
 *  - The matched volume of the order (the volume that has already been executed);
 *  If no corresponding order is found, an appropriate message should be printed.
 *  
 *  Notice that even if the order was fully executed, expired or cancelled, the order book
 *  should maintain the information about the order. Despite this requirement, such orders
 *  are not conceptually part of the order book any longer, and are not available for participating
 *  in further trades etc; they are inactive. Orders that have been deactivated never
 *  become active again.
 */
void OrderBook::print_order_info (std::ostream & where, OrderIdentifierType order_id) const
{
	// check if the orderID is valid	
	bool orderIsFound = false;	
	std::vector<AggregatedQuoteType>::size_type index = 0;
	
	for(index = 0; index < orderQueueAgainstTime.size(); index++)
	{
		if(orderQueueAgainstTime.at(index).get_number_of_orders() == order_id) 
			orderIsFound = true;
	}
	
	if(orderIsFound == false)
	{
		(*logStreamPtr) << "Order "<< order_id <<" not found." << std::endl;
		return ;
	}

	// print out order information if the order ID is valid	
	std::vector<AggregatedQuoteType>::size_type localTimeID = this->getTimeIDByOrderID(order_id);
	
    PriceType   	price          = orderQueueAgainstTime.at(localTimeID).get_price();
    VolumeType  	totalVolume    = orderQueueAgainstTime.at(localTimeID).get_total_volume();
    VolumeType  	executedVolume = orderQueueAgainstTime.at(localTimeID).get_executed_volume();
    SellBuyType 	typeOfOrder    = orderQueueAgainstTime.at(localTimeID).get_typeOfOrder();
    OrderStatusType status		   = orderQueueAgainstTime.at(localTimeID).get_status();

    
	where << "Order information: ID="<< order_id <<", type=";

	if(typeOfOrder == SellBuyType::SELL)
	{
			where << "SELL";
	}else{
			where << "BUY";
	   	 }		
	
	where << ", price=" << this->format_price(price) << ", total volume="<< totalVolume << ", executed volume=" << executedVolume << ", status=";
	
	switch (status)
	{
		case ACTIVE: 
			where <<"ACTIVE";
			break;
				
		case FILLED: 
			where <<"FILLED";
			break;
				
		case CANCELLED: 
			where <<"CANCELLED";
			break;
				
		case EXPIRED: 
			where <<"EXPIRED";
			break;
	}
	
	where <<"." << std::endl;
	
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Return a string containing the formatted version of the price price.
 *  Description:
 *  The formatting should be such that price always has exactly 
 *  1+[-log _10(tick_size)] decimal places, where d e denotes the ceiling function.
 */
std::string OrderBook::format_price (PriceType price) const
{
	// a copy of c string for supporting sprintf
	char* formattedPriceCStrPtr = new char [20];

	// calculation decimal places required to display
	// Online reference for log10()   http://www.cplusplus.com/reference/cmath/log10/
	double precision = 1+(-1)*log10(tickSize); 

	// It is not useful to compare float point number with integer by using == and therefore by using -+0.1 margin to compare with integer.	
	if( (0.9<precision) && (precision<1.1))
	{
		// Online reference for sprintf   http://www.cplusplus.com/reference/cstdio/sprintf/
		// Online reference for printf-style formatting   http://www.cplusplus.com/reference/cstdio/printf/	
		sprintf (formattedPriceCStrPtr, "%.1f", price);			
	}
	if( (1.9<precision) && (precision<2.1))
	{
		sprintf (formattedPriceCStrPtr, "%.2f", price);			
	}
	if( (2.9<precision) && (precision<3.1))
	{
		sprintf (formattedPriceCStrPtr, "%.3f", price);			
	}	
	if( (3.9<precision) && (precision<4.1))
	{
		sprintf (formattedPriceCStrPtr, "%.4f", price);			
	}	
	if( (4.9<precision) && (precision<5.1))
	{
		sprintf (formattedPriceCStrPtr, "%.5f", price);			
	}	

	// c++ std::string class for returning
	// the online manual of conversion from c string to c++ string can be found in http://www.cplusplus.com/reference/string/string/string/
	std::string formattedPriceCppStr(formattedPriceCStrPtr);
	return formattedPriceCppStr;
}

// N.B. The following description is copied from the project specification, IB9N7 C++ for Quantitative Finance Project 2015: Central limit order book implementation.
/** Get the information on the current orderbook.
 *  Desrciption:
 *  Get the information on (the active and visible portions of orders in) the current order
 *  book. Return a vector representing the requested sides of the order book (bid or ask).
 *  Each element of the vector should be of type AggregatedQuoteType, a simple class
 *  providing the following member functions (you may or may not need/want to add more):
 *  ¨C PriceType get_price()const; ¡ª price of the level;
 *  ¨C VolumeType get_volume()const; ¡ª volume available for the level;
 *  ¨C unsigned long get_number_of_orders()const;¡ªnumber of active orders
 *  of the level.
 *  The elements in the vector should be ordered according to price, so the best price comes
 *  1rst. That is, in decreasing order for bid side and in increasing order for the ask side.
 *  Note: this function should NOT generate any output (in your 1nal submission). You may
 *  use the provided display_aggregate_order_book free function in visualise.cpp
 *  to generate corresponding output, though you are also free to write (and submit) another
 *  function that outputs this information to aid with your testing if you 1nd the provided
 *  function too cumbersome. There is no prescribed formatting of the output that
 *  you need to follow in this instance.
 */
std::vector<AggregatedQuoteType> OrderBook::get_aggregated_order_book(SellBuyType which_side) const
{
	if(which_side ==  SellBuyType::SELL)
	{
		return sellOrderQueue;
	}else{
			return buyOrderQueue;
		 }
	
}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- Private Methods-------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

// Execute transactions based the order with the highest priority, including market orders and limit orders
void OrderBook::executeTransactions(void)
{
	// orderID of the order with highest priority in each orderQueue.
	OrderIdentifierType buyQueue1stOrderId, sellQueue1stOrderId;
	// volume executed in a transaction
	VolumeType			volumeExecuted;
	// various flags
	bool                marketOrderIsABuyOrder  = false;
	bool                marketOrderIsASellOrder = false;

	// A buy market order will be expired if no sell orders are available.
	if((sellOrderQueue.size() == 0) && (buyOrderQueue.size() > 0) && (buyOrderQueue.front().get_price() == 0))
	{
		buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
		orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(EXPIRED);
		buyOrderQueue.erase(buyOrderQueue.begin());		
	}

	// A sell market order will be expired if no buy orders are available.
	if((sellOrderQueue.front().get_price() == 0) && (sellOrderQueue.size() > 0) && (buyOrderQueue.size() == 0))
	{
		sellQueue1stOrderId  = sellOrderQueue.front().get_number_of_orders();
		orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(EXPIRED);
		sellOrderQueue.erase(sellOrderQueue.begin());		
	}

	// If any of queue is empty, there won't be any transaction to be executed apart from expired market orders
	// Accessing an empty queue used in comparison will introduce a run-time error.
	// It is necessary to abort OrderBook::executeTransactions(void)
	if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;

/*
	//-------------for debug usage----------------------------------------------------------------
	std::cerr << "buy1stOrder price = " << buyOrderQueue.front().get_price() << ", sell1stOrder price = " << sellOrderQueue.front().get_price() << std::endl;
	std::cerr << "buyOrderQueue size = " << buyOrderQueue.size()  << ", sell1OrderQueue size = " << sellOrderQueue.size() << std::endl;
	//---------------------------------------------------------------------------------------------
*/
	// 1. Execute transactions of market orders
	
	while( (buyOrderQueue.front().get_price() == 0) || (sellOrderQueue.front().get_price() == 0) )
	{
		if(buyOrderQueue.front().get_price() == 0)
		{
				marketOrderIsABuyOrder = true;
		}else{
				marketOrderIsASellOrder = true;
			 }
		
		// e.g. case 1 	buyOrderQueue.front has the same volume of sellOrderQueue.front()	
		//sell queue   15(v10) 19  20  25  30  
		//buy  queue   0(v10) 14  13  13  11  10
		if(buyOrderQueue.front().get_volume() == sellOrderQueue.front().get_volume())
		{
			if(marketOrderIsABuyOrder == true){
				(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 				
				(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< sellOrderQueue.front().get_volume() <<"."<<std::endl;
			}else{
				(*logStreamPtr) << "Transaction: "<<"BUY="<< buyOrderQueue.front().get_number_of_orders() << ", SELL="<< sellOrderQueue.front().get_number_of_orders() ;
				(*logStreamPtr) <<", price="<<this->format_price(buyOrderQueue.front().get_price())<<", volume="<< sellOrderQueue.front().get_volume() <<"."<<std::endl;
			}
		
			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).get_volume(); 
			// Record volume are executed
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled 
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(FILLED);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(FILLED);
			
			// these two orders are filled. Remove them from the queues.
			buyOrderQueue.erase(buyOrderQueue.begin());							
			sellOrderQueue.erase(sellOrderQueue.begin());
			
			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and both of these two orderQueue have changed.
			// the result of checking if the new order is a market order
			// from while( (buyOrderQueue.front().get_price() == 0) || (sellOrderQueue.front().get_price() == 0) )
			// is not valid any more. So redo the comparison for next transaction.
			continue;
		}

		// e.g. case 2 	buyOrderQueue.front has the more volume of sellOrderQueue.front()	
		//sell queue1   15(v10) 19  20  25  30  
		//buy  queue1   0(v15) 14  13  13  11  10
		//sell queue2   0(v10) 19  20  25  30  
		//buy  queue2   16(v15) 14  13  13  11  10				
		if(buyOrderQueue.front().get_volume() > sellOrderQueue.front().get_volume())
		{
			if(marketOrderIsABuyOrder == true){
				(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 				
				(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< sellOrderQueue.front().get_volume() <<"."<<std::endl;
			}else{
				(*logStreamPtr) << "Transaction: "<<"BUY="<< buyOrderQueue.front().get_number_of_orders() << ", SELL="<< sellOrderQueue.front().get_number_of_orders() ;
				(*logStreamPtr) <<", price="<<this->format_price(buyOrderQueue.front().get_price())<<", volume="<< sellOrderQueue.front().get_volume() <<"."<<std::endl;
			}
		
			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).get_volume(); 
			// Record volume are executed
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(FILLED); 

			// Consume the column of this order in the order queue as the transaction happens
			buyOrderQueue.front().reduceVolumeBy(sellOrderQueue.front().get_volume());
			// this order is filled. Remove it from the order queue.			
			sellOrderQueue.erase(sellOrderQueue.begin());
			
			// only when this market order is a buy type and sell order queue has no elements
			// after one or more than one transaction, this market order cannot be carried out further,
			// it should be deactivated, the total volume should be reduced to the already 
			// executed volume, and marked as filled.
			if((sellOrderQueue.size() == 0) && (marketOrderIsABuyOrder == true))
			{
				orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).completePartialFilledMarketOrder();
				orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(FILLED);
				buyOrderQueue.erase(buyOrderQueue.begin());
			} 
			
			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and one of the orderQueue has changed.
			// the result of checking if the new order is a market order
			// from while( (buyOrderQueue.front().get_price() == 0) || (sellOrderQueue.front().get_price() == 0) )
			// is not valid any more. So redo the comparison for next transaction.
			continue;
		}
		
		// e.g. case 3 	buyOrderQueue.front has the less volume of sellOrderQueue.front()	
		//sell queue1   15(v20) 19  20  25  30  
		//buy  queue1   0(v15) 14  13  13  11  10
		//sell queue2   0(v20) 19  20  25  30  
		//buy  queue2   16(v15) 14  13  13  11  10	
		if(buyOrderQueue.front().get_volume() < sellOrderQueue.front().get_volume())
		{
			if(marketOrderIsABuyOrder == true){
				(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 				
				(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< buyOrderQueue.front().get_volume() <<"."<<std::endl;
			}else{
				(*logStreamPtr) << "Transaction: "<<"BUY="<< buyOrderQueue.front().get_number_of_orders() << ", SELL="<< sellOrderQueue.front().get_number_of_orders() ;
				(*logStreamPtr) <<", price="<<this->format_price(buyOrderQueue.front().get_price())<<", volume="<< buyOrderQueue.front().get_volume() <<"."<<std::endl;
			}

			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).get_volume();
			// Record volume are executed 
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(FILLED);
			
			// Consume the volume of this order in the order queue as the transaction happens
			sellOrderQueue.front().reduceVolumeBy(buyOrderQueue.front().get_volume());
			// this order is filled. Remove it from the order queue.					
			buyOrderQueue.erase(buyOrderQueue.begin());		

			// only when this market order is a sell type and buy order queue has no elements,
			// after one or more than one transaction, this market order cannot be carried out further,
			// it should be deactivated, the total volume should be reduced to the already 
			// executed volume, and marked as filled.
			if((buyOrderQueue.size() == 0) && (marketOrderIsASellOrder == true))
			{
				orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).completePartialFilledMarketOrder();
				orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(FILLED);
				sellOrderQueue.erase(sellOrderQueue.begin());
			} 
			
			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and one of the orderQueue has changed.
			// the result of checking if the new order is a market order
			// from while( (buyOrderQueue.front().get_price() == 0) || (sellOrderQueue.front().get_price() == 0) )
			// is not valid any more. So redo the comparison for next transaction.
			continue;					
		}		

	} // end of 1. Execute transactions of market orders
		
	// 2. Execute transactions of limit orders

	// If the price of the order with highest priority in the buy queue is greater than or equal to
	// the price of the order with highest priority in the sell queue.
	// Then the transaction will be executed.
	// e.g. buy 15 >= sell 5
	while(this->compareTwoOrdersPrice(buyOrderQueue.front().get_price(), sellOrderQueue.front().get_price()) != LESS )
	{
		// e.g. case 1 	buyOrderQueue.front has the same volume of sellOrderQueue.front()	
		//sell queue   15(v10) 19  20  25  30  
		//buy  queue   18(v10) 14  13  13  11  10
		if(buyOrderQueue.front().get_volume() == sellOrderQueue.front().get_volume())
		{
			(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 
			(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< buyOrderQueue.front().get_volume() <<"."<<std::endl;
		
			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).get_volume(); 
			// Record volume are executed
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled 
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(FILLED);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(FILLED);
			
			// these two orders are filled. Remove them from the queues.
			buyOrderQueue.erase(buyOrderQueue.begin());							
			sellOrderQueue.erase(sellOrderQueue.begin());

			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and both of these two orderQueue have changed.
			// comparison results from while(this->compareTwoOrdersPrice(buyOrderQueue.front().get_price(), sellOrderQueue.front().get_price()) != LESS )
			// is not valid any more. So redo the comparison for next transaction.
			continue;
		}
		
		// e.g. case 2 	buyOrderQueue.front has the more volume of sellOrderQueue.front()	
		//sell queue   15(v10) 19  20  25  30  
		//buy  queue   18(v15) 14  13  13  11  10		
		if(buyOrderQueue.front().get_volume() > sellOrderQueue.front().get_volume())
		{
			(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 
			(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< sellOrderQueue.front().get_volume() <<"."<<std::endl;
		
			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).get_volume(); 
			// Record volume are executed
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).changeStatusTo(FILLED); 

			// Consume the volume of this order in the order queue as the transaction happens
			buyOrderQueue.front().reduceVolumeBy(sellOrderQueue.front().get_volume());
			// this order is filled. Remove it from the order queue.			
			sellOrderQueue.erase(sellOrderQueue.begin());
			
			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and one of the orderQueue has changed.
			// comparison results from while(this->compareTwoOrdersPrice(buyOrderQueue.front().get_price(), sellOrderQueue.front().get_price()) != LESS )
			// is not valid any more. So redo the comparison for next transaction.
			continue;
		}

		// e.g. case 3 	buyOrderQueue.front has the less volume of sellOrderQueue.front()	
		//sell queue   15(v20) 19  20  25  30  
		//buy  queue   18(v15) 14  13  13  11  10	
		if(buyOrderQueue.front().get_volume() < sellOrderQueue.front().get_volume())
		{
			(*logStreamPtr) << "Transaction: SELL="<< sellOrderQueue.front().get_number_of_orders() <<", BUY="<< buyOrderQueue.front().get_number_of_orders(); 
			(*logStreamPtr) <<", price="<<this->format_price(sellOrderQueue.front().get_price())<<", volume="<< buyOrderQueue.front().get_volume() <<"."<<std::endl;

			// Record this transaction into orderQueueAgainstTime
			buyQueue1stOrderId  = buyOrderQueue.front().get_number_of_orders();
			sellQueue1stOrderId = sellOrderQueue.front().get_number_of_orders();
			volumeExecuted 		= orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).get_volume();
			// Record volume are executed 
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(sellQueue1stOrderId)).reduceVolumeBy(volumeExecuted);
			// Record status filled
			orderQueueAgainstTime.at(this->getTimeIDByOrderID(buyQueue1stOrderId)).changeStatusTo(FILLED);
			
			// Consume the volume of this order in the order queue as the transaction happens
			sellOrderQueue.front().reduceVolumeBy(buyOrderQueue.front().get_volume());
			// this order is filled. Remove it from the order queue.					
			buyOrderQueue.erase(buyOrderQueue.begin());		

			// If any of queue is empty, there won't be any transaction to be executed
			// Accessing an empty queue used in comparison will introduce a run-time error.
			// It is necessary to abort OrderBook::executeTransactions(void)
			if( (buyOrderQueue.size() == 0) || (sellOrderQueue.size() == 0)) return;
			// This transaction has been executed, and one of the orderQueue has changed.
			// comparison results from while(this->compareTwoOrdersPrice(buyOrderQueue.front().get_price(), sellOrderQueue.front().get_price()) != LESS )
			// is not valid any more. So redo the comparison for next transaction.
			continue;
		}
	}// end of 2. Execute transactions of limit orders 	
}

/** Compare prices of two orders
 *  Description:
 *  Compare prices of two orders, return CompareResultType GREATER, EQUAL, or LESS. 
 *  The order book should treat prices  which differ by ¡Ü 2*tolerance as being equal. 
 *  This applies for both the price-time priority principle and the get_aggregated_order_book function.
 */
CompareResultType OrderBook::compareTwoOrdersPrice(PriceType price1, PriceType price2)
{
	
	PriceType difference;
	bool      diffIsPositive = true;

	if(price1 > price2)
	{
		diffIsPositive = true;
		difference = price1 - price2;
	}else{
		diffIsPositive = false;
		difference = price2 - price1;		
	}
	
	
	if(diffIsPositive == true)
	{
		//leave a little margin rather than just 2*tolerance due to the uncertainty of comparison between float point numbers 
		if(difference < 2.1*tolerance) 
		{
			return EQUAL;
		}else{
			return GREATER;
		}		
	}else{
		//leave a little margin rather than just 2*tolerance due to the uncertainty of comparison between float point numbers 
		if(difference < 2.1*tolerance) 
		{
			return EQUAL;
		}else{
			return LESS;
		}
	}
}

// For debug usage only
// It prints sell or buy order queue in the order of priority 
void OrderBook::print_order_queue_info (SellBuyType typeOfOrderQueue) const
{
    PriceType   price;
    VolumeType  volume;
    OrderIdentifierType orderID;
	std::vector<AggregatedQuoteType>::size_type index;

	if(typeOfOrderQueue == SellBuyType::SELL)
	{
		for(index = 0; index < sellOrderQueue.size(); index++)	
		{
		    price        = sellOrderQueue.at(index).get_price();
		    volume       = sellOrderQueue.at(index).get_volume();
		    orderID      = sellOrderQueue.at(index).get_number_of_orders();
			// Online reference for std::ios_base::precision  http://www.cplusplus.com/reference/ios/ios_base/precision/
			// Online reference for std::fixed  http://www.cplusplus.com/reference/ios/fixed/
			std::cerr.precision(2);		    
		    std::cerr << "[for debug usage] Order: ID="<<orderID<<", type=SELL, price="<<std::fixed<<price<<", volume="<<volume<<"." << std::endl;
		}
	}
	
	if(typeOfOrderQueue == SellBuyType::BUY)
	{
		for(index = 0; index < buyOrderQueue.size(); index++)	
		{
		    price        = buyOrderQueue.at(index).get_price();
		    volume       = buyOrderQueue.at(index).get_volume();
		    orderID      = buyOrderQueue.at(index).get_number_of_orders();		    
			// Online reference for std::ios_base::precision  http://www.cplusplus.com/reference/ios/ios_base/precision/
			// Online reference for std::fixed  http://www.cplusplus.com/reference/ios/fixed/
			std::cerr.precision(2);		    
		    std::cerr << "[for debug usage] Order: ID="<<orderID<<", type=BUY, price="<<std::fixed<<price<<", volume="<<volume<<"." << std::endl;
		}
	}	 
}

// print out the information of the order just been submitted 
void OrderBook::print_order_submitting_info (OrderIdentifierType order_id) const
{
	std::vector<AggregatedQuoteType>::size_type localTimeID = this->getTimeIDByOrderID(order_id);
	
    PriceType   price        = orderQueueAgainstTime.at(localTimeID).get_price();
    VolumeType  volume       = orderQueueAgainstTime.at(localTimeID).get_total_volume();
    SellBuyType typeOfOrder  = orderQueueAgainstTime.at(localTimeID).get_typeOfOrder();
	
	if(typeOfOrder == SellBuyType::SELL)
	{
			(*logStreamPtr) << "Order submitted: ID="<<order_id<<", type=SELL, price="<<this->format_price(price)<<", volume="<<volume<<"." << std::endl;
	}else{
			(*logStreamPtr) << "Order submitted: ID="<<order_id<<", type=BUY, price="<<this->format_price(price)<<", volume="<<volume<<"." << std::endl;
	   	 }	
}

// For debug usage only
// It prints all orders in the order of time that they are sumbitted. 
void OrderBook::printAllOrdersInfoAgainstTime (void) const
{
	OrderIdentifierType orderID = 0;
	std::vector<AggregatedQuoteType>::size_type index = 0;

	for(index = 0; index < orderQueueAgainstTime.size(); index++)
	{
		orderID++;
		this->print_order_info( std::cerr, orderID);
	}			
}

// Walking through queue orderQueueAgainstTime to find an timeID of an order for indexing by using its orderID.  
std::vector<AggregatedQuoteType>::size_type OrderBook::getTimeIDByOrderID(OrderIdentifierType orderID) const
{
	std::vector<AggregatedQuoteType>::size_type timeID_r = 0;
	
	for(timeID_r = 0; timeID_r < orderQueueAgainstTime.size(); timeID_r++)
	{
		if(orderQueueAgainstTime.at(timeID_r).get_number_of_orders() == orderID) 
			return timeID_r;
	}
	
	// should never reach here as long as orderID is valid
	return -1;		
}





