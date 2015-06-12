// My University number is: *** to be filled in ***
#ifndef CLOB_SHARED_HPP
#define CLOB_SHARED_HPP

/*
	Setting up for Dev-C++ software:
	1. Download latest version Dev-Cpp 5.10 TDM-GCC 4.8.1 Setup.exe from http://vorboss.dl.sourceforge.net/project/orwelldevcpp/Setup%20Releases/Dev-Cpp%205.10%20TDM-GCC%204.8.1%20Setup.exe and install the (full) version of the package
	2. Double click on CLOB.dev in ib9n70_project_2015_starting_code folder to open project.
	3. Go to menu "Project" -> "Project Options"
	4. Under "Parameter" tag, add parameter "-std=c++11" under "C++ compiler" to support C++11 standard.
	5. Compile the project and see if there are any errors.
*/


// predefined strongly typed enum. This is not only in user program but in visualise.cpp as well. It is necessary to keep this class definition.
// This is only supported by C++11. If the IDE cannot parse this statement, the solution is to download the latest version of Dev-C++,
// which supports C++11, and turn on -std=c++11 switch in compiler options as well.
enum class SellBuyType {SELL, BUY};

// predefined strongly typed enum are not handy to operate when it is used for status rather than just constant. Using a workaround by define OrderStatusType 
// to a common enum type.
typedef enum {ACTIVE, FILLED, CANCELLED, EXPIRED} OrderStatusType;
// enum class OrderStatusType {ACTIVE, FILLED, CANCELLED, EXPIRED};

// enum class presenting the result of comparasion between the price of two orders
// used in method CompareResultType OrderBook::compareTwoOrdersPrice(PriceType price1, PriceType price2) in OrderBook.cpp
typedef enum {GREATER, EQUAL, LESS} CompareResultType;
 


typedef unsigned long OrderIdentifierType;
typedef double PriceType;
typedef unsigned long VolumeType;
typedef unsigned long TimeType;





#endif // CLOB_SHARED_HPP
