// My University number is: *** to be filled in ***
#ifndef AGGREGATED_QUOTE_TYPE_HPP
#define AGGREGATED_QUOTE_TYPE_HPP

#include "CLOB_shared.hpp"

class AggregatedQuoteType
{
	public:
        // user-defined constructor   
        AggregatedQuoteType(OrderIdentifierType orderIDSet, SellBuyType typeOfOrderSet, PriceType priceSet, VolumeType volumeSet);
        
        // user-defined mutators
        void changeStatusTo(OrderStatusType newStatus);
        void reduceVolumeBy(VolumeType volumeReduced);
        void cancelPartialFilledMarketOrder(void);
		void completePartialFilledMarketOrder(void);
		void closePartialFilledMarketOrder(void);
        
	
        // predefined accessors --- N.B. must not be removed, they are also used in visualise.cpp as well.
		OrderIdentifierType 	get_number_of_orders() const;
		PriceType 				get_price() 		   const;
		// get the volume left able to be executed
		VolumeType 				get_volume() 		   const;
		
		// user-defined accessors
		OrderStatusType 		get_status() 		   const;
		SellBuyType 			get_typeOfOrder()      const;
		VolumeType 				get_executed_volume()  const;
		VolumeType 				get_total_volume()     const;
				
		
	private:
		
		// varialbes for 
        OrderIdentifierType orderID;
        SellBuyType         typeOfOrder;
        PriceType           price;
        VolumeType          totalVolume, volumeLeft, executedVolume;
        OrderStatusType     orderStatus;
};



#endif // AGGREGATED_QUOTE_TYPE_HPP
