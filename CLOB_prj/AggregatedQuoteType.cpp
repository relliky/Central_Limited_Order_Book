// My University number is: *** to be filled in ***
#include "AggregatedQuoteType.hpp"

#include <iostream>

AggregatedQuoteType::AggregatedQuoteType(OrderIdentifierType orderIDSet, SellBuyType typeOfOrderSet, PriceType priceSet, VolumeType volumeSet)
{
    orderID     = orderIDSet;
    typeOfOrder = typeOfOrderSet;
    price       = priceSet;
    orderStatus = ACTIVE;
	                       
	totalVolume = volumeSet;
	volumeLeft  = volumeSet;
	executedVolume = 0;    
}

void AggregatedQuoteType::changeStatusTo(OrderStatusType newStatus)
{
    orderStatus = newStatus;                             
}

void AggregatedQuoteType::reduceVolumeBy(VolumeType volumeReduced)
{
    volumeLeft      -= volumeReduced;
	executedVolume  += volumeReduced;
//	std::cerr << "[DEBUG] order ID=" << orderID << ", volumeLeft =" << volumeLeft <<", totalVolume =" << totalVolume << ", executedVolume =" <<executedVolume <<std::endl;
}
 
void AggregatedQuoteType::cancelPartialFilledMarketOrder(void)
{
	totalVolume = executedVolume;
	volumeLeft  = 0;	
}

void AggregatedQuoteType::completePartialFilledMarketOrder(void)
{
	totalVolume = executedVolume;
	volumeLeft  = 0;	
}
 
void AggregatedQuoteType::closePartialFilledMarketOrder(void)
{
	totalVolume = executedVolume;
	volumeLeft  = 0;	
}
 
PriceType AggregatedQuoteType::get_price() const
{
	return price;
}

VolumeType AggregatedQuoteType::get_volume() const
{
	return volumeLeft;
}

OrderIdentifierType AggregatedQuoteType::get_number_of_orders() const
{
	return orderID;
}

OrderStatusType AggregatedQuoteType::get_status() const
{
    return orderStatus;                
}

SellBuyType AggregatedQuoteType::get_typeOfOrder() const
{
	return typeOfOrder;
}

VolumeType AggregatedQuoteType::get_executed_volume() const
{
	return executedVolume;
}

VolumeType AggregatedQuoteType::get_total_volume() const
{
	return totalVolume;
}

		
