/*
* author - Maks Cegielski-Johnson
* author - John Ballard
* CS 3505 - Assignment 4
* Warehouse object, encapsulates a warehouse that contains a collection of
* dated items that can expire.
*/

#include <string>
#include <vector>
#include <iostream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/foreach.hpp>
#include "warehouse.h"
#include "item.h"
#include "dated_item.h"

using namespace std;
using namespace boost::gregorian;

// Constructor for a warehouse. Saves the given name as the city name.
warehouse::warehouse(string name)
{
	city_name = name;
}

// Copy constructor - copies another warehouse.
warehouse::warehouse(const warehouse &other)
: city_name(other.city_name), inventory(other.inventory)
{
}

// Default constructor. Creates a warehouse with no name.
warehouse::warehouse()
{
	city_name = "";
}

// Destructor
warehouse::~warehouse()
{
	
}

// Gets the name of this warehouse.
string warehouse::get_name()
{
	return city_name;
}

// Receives a given quantity of a given item on a given date.
void warehouse::receive(item new_item, date current_date, int quantity)
{
	// Get the date the item expires.
	date expiration_date = new_item.get_expiration_date(current_date);
	BOOST_FOREACH(dated_item & cur, inventory)
	{
		// If we find a dated item with the same item and expiration date, just increase the quantity of that item.
		if(cur.get_item() == new_item && cur.get_date() == expiration_date)
		{
			cur.add(quantity);
			return;
		}
	}
	// Add a dated item containing the item to the inventory.
	dated_item d_item(new_item, expiration_date, quantity);
	inventory.push_back(d_item);
}

// Reqests the given quantity of the given item. If there is less of the item than is requested,
// the remainder of the request is ignored.
void warehouse::request(item requested_item, int quantity)
{
	bool done_removing = false;
	while(!done_removing)
	{
		// If the warehouse does not contain the item, we are done.
		if(!contains(requested_item))
		{
			done_removing = true;
			break;
		}
		// Get the date of the oldest item in the inventory.
		date oldest_date = get_soonest_date(requested_item);
		// Get the quantity of the item with that date that are in the inventory.
		int num_in_inventory = get_quantity(requested_item, oldest_date);
		// Remove the requested quantity from the inventory.
		remove_item(requested_item, oldest_date, quantity);
		
		// If the number in the inventory with that date is less than the quantity remaining, decrease the quantity
		// by the number in the inventory.
		if(num_in_inventory < quantity)
		{
			quantity -= num_in_inventory;
		}
		// Otherwise, we are done.
		else
		{
			done_removing = true;
		}
	
	}
}

// Returns true if the warehouse contains and amount of the given item and false otherwise.
bool warehouse::contains(item requested_item)
{
	BOOST_FOREACH(dated_item cur_item, inventory)
	{
		if (cur_item.get_item() == requested_item)
		{
			return true;
		}
	}
	return false;
}

// Removes all items from the warehouse that expire on or before the given date.
void warehouse::clear_expired_items(date current_date)
{
	// Loop through the inventory and remove all the items with a date at or before the current date.
	vector<dated_item>::iterator it = inventory.begin();
	while(it != inventory.end())
	{
		if((*it).get_date() <= current_date)
		{
			it = inventory.erase(it);
		}
		else
		{
			it++;
		}
	}
}

// Overloaded assignment operator. Copies another warehouse.
warehouse& warehouse::operator=(const warehouse &rhs)
{
	city_name = rhs.city_name;
	inventory = rhs.inventory;
	return *this;
}

// Removes the given quantity of the given item with the given expiration date.
// If the item would have a 0 or negative quantity after the removal, the item is removed from the inventory vector
void warehouse::remove_item(item requested_item, date expiration_date, int quantity)
{
	// Loop through the vector
	for(vector<dated_item>::iterator it = inventory.begin(); it != inventory.end(); it++)
	{
		// If the dated_item has the correct item and date, remove the appropriate amount
		if((*it).get_item() == requested_item && (*it).get_date() == expiration_date)
		{
			if((*it).get_quantity() <= quantity)
			{
				inventory.erase(it);
			}
			else
			{
				(*it).remove(quantity);
			}
			break;
		}
	}
}

// Gets the quantity of the given item at the given date that exists in the warehouse.
int warehouse::get_quantity(item requested_item, date expiration_date)
{
	BOOST_FOREACH(dated_item _item, inventory)
	{
		if(_item.get_item() == requested_item && _item.get_date() == expiration_date)
		{
			return _item.get_quantity();
		}
	}
	return 0;
}


// Gets the soonest (farthest back in time) date that any item matching the given item
// will expire. Returns date(not_a_date_time) if the item is not found.
date warehouse::get_soonest_date(item requested_item)
{
	date soonest_date;
	BOOST_FOREACH(dated_item _item, inventory)
	{
		if(_item.get_item() == requested_item)
		{
			if(soonest_date.is_not_a_date() || _item.get_date() < soonest_date)
			{
				soonest_date = _item.get_date();
			}
		}
	}
	return soonest_date;
}

// Debugging method. Prints a summary of all the items in the warehouse.
void warehouse::print_summary()
{
	cout << city_name << " contains:" << endl;
	BOOST_FOREACH(dated_item cur, inventory)
	{
		cout << cur.get_quantity() << " of " << cur.get_item().get_name() << endl;
	}
	cout << "----------------------------------------" << endl;
	cout << endl;
}