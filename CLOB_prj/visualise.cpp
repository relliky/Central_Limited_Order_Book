#include <sstream>
#include <iostream>
#include "OrderBook.hpp"
#include "visualise.hpp"

#include <limits>

void pause()
{
	std::cout << "Press ENTER to continue..." << std::flush;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
std::string to_string_via_sstream(T x)
{
	std::stringstream s;
	s << x;
	return s.str();
}


// #include "CImg.h"

// namespace
// {
	// const unsigned int image_width = 1024u;
	// const unsigned int image_height = 768u;
	
	// cimg_library::CImgDisplay disp(image_width, image_height, "Aggregate order book", 0, false, true);
// }

void display_aggregate_order_book_graphics(const OrderBook & ob)
{
	
	// // aggregate order book
	
	// std::vector<AggregatedQuoteType> aggBUY(ob.get_aggregated_order_book(SellBuyType::BUY)), aggSELL(ob.get_aggregated_order_book(SellBuyType::SELL));
	
	// cimg_library::CImg<unsigned int> image(/*x*/image_width,/*y*/image_height,/*z*/1u,/*channels*/3u,/*initial fill*/0u);
	

	// const unsigned int row_height = 40u;
	// const unsigned int row_gap = 10u;
	// const unsigned int col_width = 50u;
	// const unsigned int col_sep = 10u;
	// const unsigned int bar_col_width = image_width / 2u - 3u * (col_width + col_sep);
	
	// const unsigned int green[] = { 0u,255u,0u };
	// const unsigned int red[] = { 255u,0u,0u };
	// const unsigned int white[] = { 255u,255u,255u };
	
	// unsigned int row_start = 5u;
	
	// const unsigned int buy_col1 = bar_col_width + col_sep;
	// const unsigned int buy_col2 = buy_col1 + col_width + col_sep;
	// const unsigned int buy_col3 = buy_col2 + col_width + col_sep;
	
	// const unsigned int divider_x = image_width / 2u;
	
	// const unsigned int sell_col1 = divider_x + col_sep;
	// const unsigned int sell_col2 = sell_col1 + col_width + col_sep;
	// const unsigned int sell_col3 = sell_col2 + col_width + col_sep;
	
	// image.draw_line(divider_x, 0u, divider_x, image_height, white);
	
	
	// image.draw_text(buy_col1, row_start, "BUY orders", white, 0);
	// image.draw_text(sell_col1, row_start, "SELL orders", white, 0);
	
	// row_start += row_height + row_gap;
	
	// image.draw_text(buy_col1, row_start, "# orders", white, 0);
	// image.draw_text(buy_col2, row_start, "Volume", white, 0);
	// image.draw_text(buy_col3, row_start, "Price", white, 0);

	// image.draw_text(sell_col1, row_start, "# orders", white, 0);
	// image.draw_text(sell_col2, row_start, "Volume", white, 0);
	// image.draw_text(sell_col3, row_start, "Price", white, 0);
	
	// for (auto iterBUY = aggBUY.begin(), iterSELL = aggSELL.begin(); iterBUY != aggBUY.end() || iterSELL != aggSELL.end(); ((iterBUY != aggBUY.end()) ? (++iterBUY) : iterBUY), ((iterSELL != aggSELL.end()) ? (++iterSELL) : iterSELL))
	// {
		// row_start += row_height + row_gap;
		
		// if (iterBUY != aggBUY.end())
		// {
			// image.draw_rectangle(bar_col_width - iterBUY->get_volume(), row_start, bar_col_width, row_start + row_height, green);
		
			// image.draw_text(buy_col1, row_start, to_string_via_sstream(iterBUY->get_number_of_orders()).c_str(), white, 0);
			// image.draw_text(buy_col2, row_start, to_string_via_sstream(iterBUY->get_volume()).c_str(), white, 0);
			// image.draw_text(buy_col3, row_start, ob.format_price(iterBUY->get_price()).c_str(), white, 0);
		// }
		// else
		// {
			
		// }
		
		// if (iterSELL != aggSELL.end())
		// {
			// image.draw_rectangle(image_width - bar_col_width, row_start, image_width - bar_col_width + iterSELL->get_volume(), row_start + row_height, red);
			
			// image.draw_text(sell_col1, row_start, to_string_via_sstream(iterSELL->get_number_of_orders()).c_str(), white, 0);
			// image.draw_text(sell_col2, row_start, to_string_via_sstream(iterSELL->get_volume()).c_str(), white, 0);
			// image.draw_text(sell_col3, row_start, ob.format_price(iterSELL->get_price()).c_str(), white, 0);
		// }
		// else
		// {
			
		// }
		
		
		
	// }
	
	// //image.display("Aggregrate order book", false);
	// //cimg_library::CImgDisplay disp(image, "Aggregate order book");
	// disp.display(image);
	// disp.show();
	// while (!disp.is_closed() && !disp.key()) { disp.wait(); }
	// if (disp.is_keyQ())
		// exit(1u);
	// //disp.wait(20u);
	// disp.flush();
	
}
#include <iomanip>

void display_aggregate_order_book_console(const OrderBook & ob)
{
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "*** Aggregated order book follows: " << std::endl;
	
	std::cout << std::setw(40u) << std::left << "BUY side";
	std::cout << std::left << "SELL side";
	std::cout << std::endl;
	
	std::cout << std::setw(10u) << std::left << "# orders";
	std::cout << std::setw(10u) << std::left << "Volume";
	std::cout << std::setw(20u) << std::left << "Price";
	std::cout << std::setw(10u) << std::left << "# orders";
	std::cout << std::setw(10u) << std::left << "Volume";
	std::cout << std::left << "Price";
	std::cout << std::endl;
	
	// aggregate order book
	std::vector<AggregatedQuoteType> aggBUY(ob.get_aggregated_order_book(SellBuyType::BUY)), aggSELL(ob.get_aggregated_order_book(SellBuyType::SELL));
	
	//const std::size_t col_width = 35u;
	for (auto iterBUY = aggBUY.begin(), iterSELL = aggSELL.begin(); iterBUY != aggBUY.end() || iterSELL != aggSELL.end(); ((iterBUY != aggBUY.end()) ? (++iterBUY) : iterBUY), ((iterSELL != aggSELL.end()) ? (++iterSELL) : iterSELL))
	{
		//std::string entry;
		if (iterBUY != aggBUY.end())
		{
			std::cout << std::setw(10u) << std::left << to_string_via_sstream(iterBUY->get_number_of_orders());
			std::cout << std::setw(10u) << std::left << to_string_via_sstream(iterBUY->get_volume());
			std::cout << std::setw(20u) << std::left << ob.format_price(iterBUY->get_price());
			//entry = to_string_via_sstream(iterBUY->get_number_of_orders()) + "\t" + to_string_via_sstream(iterBUY->get_volume()) + "\t" + ob.format_price(iterBUY->get_price());
		}
		else
		{
			std::cout << std::setw(40u) << "";
			//entry = "";
		}
		//std::cout << entry << std::string(col_width - entry.length(), ' ');
		
		if (iterSELL != aggSELL.end())
		{
			std::cout << std::setw(10u) << std::left << to_string_via_sstream(iterSELL->get_number_of_orders());
			std::cout << std::setw(10u) << std::left << to_string_via_sstream(iterSELL->get_volume());
			std::cout << std::left << ob.format_price(iterSELL->get_price());
			//entry = to_string_via_sstream(iterSELL->get_number_of_orders()) + "\t" + to_string_via_sstream(iterSELL->get_volume()) + "\t" + ob.format_price(iterSELL->get_price());
		}
		else
		{
			//entry = "";
		}
		//std::cout << entry << std::string(col_width - entry.length(), ' ');
		std::cout << std::endl;
		
	}
	std::cout << std::endl;
	pause();
}

#ifdef AM_ADAM
#include <fstream>

void dump_tex_aggregated_order_book(std::ostream &out, const OrderBook & ob);

namespace
{
	std::ofstream orderoutputf("orderbook.out.txt");
}

void display_aggregate_order_book_tex(const OrderBook & ob)
{
	dump_tex_aggregated_order_book(orderoutputf, ob);
}
#else
void display_aggregate_order_book_tex(const OrderBook &)
{
	// not implemented for anyone but adam...!
}
#endif

void display_aggregate_order_book_nothing(const OrderBook &)
{
	// do nothing

}

void display_aggregate_order_book(const OrderBook & ob, DisplayType dt)
{
	switch(dt)
	{
		case DisplayType::CONSOLE:
			display_aggregate_order_book_console(ob);
			break;
		case DisplayType::GRAPHICS:
			display_aggregate_order_book_graphics(ob);
			break;
		case DisplayType::TEX:
			display_aggregate_order_book_tex(ob);
			break;
		case DisplayType::NONE:
			display_aggregate_order_book_nothing(ob);
			break;
	}
}

