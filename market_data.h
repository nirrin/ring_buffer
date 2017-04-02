//(C) 2010, 2011, 2012 Chronos Research LLC. All Rights Reserved.

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <boost/functional/hash.hpp>
#include "types.h"
#include <iostream>
#include <string>
#include <string.h>
#include <limits>

//#define  MAX_NUM_TIERS 30
// SE551 - change max to 50
#define  MAX_NUM_TIERS 50
#define MAX_NUM_TIERS_PER_LP 10

#define CROSSER_ORIGIN 0
#define SOR_RESERVATION_ORIGN 254
#define CROSSER_NAME "INT"
#define RESERVATION_NAME "RES"
#define INDICATIVE_ORIGIN 127

#define AON_ATTRIBUTE   1 //0b00000001
#define MKT_ATTRIBUTE   2 //0b00000010
#define RHD_ATTRIBUTE   4 //0b00000100
#define REAL_ATTRIBUTE  8 //0b00001000
#define BID_ATTRIBUTE  16 //0b00010000
#define USED_ATTRIBUTE 32 //0b00100000	ECN uses this to mark a tier that was already used for an order

#define MEGA_PRICE_FACTOR 1000000.0

struct CombinedMarketDataHeader
{
	ClientInstrumentId client_instrument_id;
	ExchangeId ecn;
	ExchangeId origin; // who is the originator of the book either Brutalis id or CROSSER (=0) or SOR reservation (=254)
	uint8_t brutalis_id;
	uint8_t bid_num_tiers;
	uint8_t ask_num_tiers;
	uint8_t retransmission;	// should be bool, but the size of a bool is implementation defined 
	uint32_t sequence_num;	// note: aggregator will reassign sequence_num
	uint64_t unique_id;		// TODO
	uint64_t ts_lp_out;
	uint64_t ts_connector_in;
	uint64_t ts_connector_in_wall; // the wall clock brother of ts_connector_in
	uint64_t ts_connector_q_put;
	uint64_t ts_mcast_q_get;
	uint64_t ts_mcast_out;
	uint64_t ts_agg_in;
	uint64_t ts_agg_out;
	uint64_t ts_aggpublisher_in; // aggpublisher recieve timestamp

	inline uint64_t get_unique_id() const
	{
		return unique_id;
	}

	inline ClientInstrumentId get_instrument_id() const
	{
		return client_instrument_id;
	}

	inline ExchangeId get_origin() const
	{
		return origin;
	}

	inline ExchangeId get_ecn() const
	{
		return ecn;
	}

	inline ExchangeId get_brutalis_id() const
	{
		return brutalis_id;
	}

	inline bool is_retransmit() const
	{
		return retransmission;
	}

	inline uint64_t end_to_end_latency(const uint64_t& recieve) const
	{
		return (recieve - ts_connector_in); //TODO: Check with Nir
	}

	inline void retransmit()
	{
		retransmission = 1;
	}
};

#define VALUE_DATE_LEN 8

typedef struct
{
	int32_t mega_price;
	uint32_t min_qty;
	uint32_t max_qty;
	ExchangeId ecn;
	uint8_t internal_order_attribute;
	uint8_t mde_index;
	char quote_id[71];
	char formatted_connector_timestamp[22]; // from tag 52
	char value_date[VALUE_DATE_LEN]; // should be filled by the Brutalis / Aggregator
	uint32_t desk_id;		// also used by amount reservation
	uint32_t client_id;		// also used by amount reservation
	uint32_t qty_to_send;	// used by SOR matching algorithm
	uint64_t quote_req_id;
	uint64_t connector_timestamp;

	inline bool is_aon() const
	{
		return (internal_order_attribute & AON_ATTRIBUTE);
	}

	inline bool is_mkt() const
	{
		return (internal_order_attribute & MKT_ATTRIBUTE);
	}

	inline bool is_rhd() const
	{
		return (internal_order_attribute & RHD_ATTRIBUTE);
	}

	inline bool is_real() const
	{
		return (internal_order_attribute & REAL_ATTRIBUTE);
	}

	inline bool is_bid() const
	{
		return (internal_order_attribute & BID_ATTRIBUTE);
	}

	inline bool is_used() const
	{
		return (internal_order_attribute & USED_ATTRIBUTE);
	}

	inline void aon()
	{
		internal_order_attribute |= AON_ATTRIBUTE;
	}

	inline void mkt()
	{
		internal_order_attribute |= MKT_ATTRIBUTE;
	}

	inline void rhd()
	{
		internal_order_attribute |= RHD_ATTRIBUTE;
	}

	inline void real()
	{
		internal_order_attribute |= REAL_ATTRIBUTE;
	}

	inline void bid()
	{
		//internal_order_attribute |= BID_ATTRIBUTE;
		internal_order_attribute = BID_ATTRIBUTE;
	}

	inline void ask()
	{
		internal_order_attribute = 0;
	}

	inline void used()
	{
		internal_order_attribute |= USED_ATTRIBUTE;
	}

	inline void not_used()
	{
		internal_order_attribute &= (uint8_t) ~USED_ATTRIBUTE;
	}

	inline double price() const
	{
		return (mega_price / MEGA_PRICE_FACTOR);
	}

	inline std::string get_quote_id() const
    {
        if (quote_id[70] != 0)            
            return std::string(quote_id, 71);
        else
            return std::string(quote_id);
    }

    inline std::string get_formatted_connector_timestamp() const
    {
        if (formatted_connector_timestamp[21] != 0)
            return std::string(formatted_connector_timestamp, 22);
        else
            return std::string(formatted_connector_timestamp);
    }

    inline std::string get_value_date() const
    {
        return std::string(value_date, VALUE_DATE_LEN);
    }
		
	inline bool has_value_date() const
	{
		const char *s = value_date;
		const uint64_t *ui = (const uint64_t*) s;
		return (*ui != 0);
	}
}
Tier;

struct CombinedSortedBook
{

	struct RateKey
	{
		ClientInstrumentId instrument;
		ExchangeId ecn;

		RateKey()
		{
			instrument = 0;
			ecn = 0;
		}

		RateKey(CombinedSortedBook *book, bool)
		: instrument(book -> header.client_instrument_id), ecn(book -> header.ecn)
		{
		}

		RateKey(ClientInstrumentId _instrument, ExchangeId _ecn)
		: instrument(_instrument), ecn(_ecn)
		{
		}

		RateKey(const RateKey& rateTimestamp)
		: instrument(rateTimestamp.instrument), ecn(rateTimestamp.ecn)
		{
		}

		inline void init(CombinedSortedBook *book)
		{
			instrument = book -> header.client_instrument_id;
			ecn = book -> header.ecn;
		}

		inline bool shouldCheckLostRate() const
		{
			return (ecn != SOR_RESERVATION_ORIGN);
		}

		friend bool operator==(const RateKey &l, const RateKey &r)
		{
			return l.instrument == r.instrument && l.ecn == r.ecn;
		}

		friend std::size_t hash_value(const RateKey &t)
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, t.instrument);
			boost::hash_combine(seed, t.ecn);

			return seed;
		}

		uint32_t key_for_sequence_check() const
		{
			return ((uint32_t) ecn * 10000 + (uint32_t) instrument);
		}

		template <typename S>
		friend S& operator<<(S& os, const RateKey& key)
		{
			os << "[Instrument: " << key.instrument << "][ECN: " << key.ecn << "]";
			return os;
		}

	};

	CombinedMarketDataHeader header;
	Tier tier[MAX_NUM_TIERS * 2];

#ifdef CPU_STATS
	// These timetamps are not sent, they are used for Brutalis statistics
	uint64_t cpu_ts_connector_in;
	uint64_t cpu_ts_connector_q_put;
	uint64_t cpu_ts_mcast_q_get;
	uint64_t cpu_ts_mcast_out;
#endif

	CombinedSortedBook()
	{
		memset((void*) this, 0, sizeof (CombinedSortedBook));
	}

	CombinedSortedBook(const RateKey& key)
	{
		memset((void*) this, 0, sizeof (CombinedSortedBook));
		header.client_instrument_id = key.instrument;
		header.ecn = key.ecn;
	}

	inline static size_t HeaderSize()
	{
		return (sizeof (CombinedMarketDataHeader));
	}

	size_t TransmitSize() const
	{
		return sizeof (header) + (header.bid_num_tiers + header.ask_num_tiers) * sizeof (Tier);
	}

	bool should_retransmit() const
	{
		return (header.ecn != SOR_RESERVATION_ORIGN);
	}

	inline bool valid_sides() const
	{
		for (uint8_t i = 0; i < header.bid_num_tiers; i++) {
			if (!tier[i].is_bid()) {
				return false;
			}
		}
		for (uint8_t i = header.bid_num_tiers; i < header.bid_num_tiers + header.ask_num_tiers; i++) {
			if (tier[i].is_bid()) {
				return false;
			}
		}
		return true;
	}

	inline char *value_date()
	{
		if (header.bid_num_tiers > 0 || header.ask_num_tiers > 0)
			return tier[0].value_date;
		else
			return NULL;
	}

	inline uint64_t value_date_to_uint64()
	{
		char *vd = value_date();
		if (vd != NULL) {
			uint64_t *ui = (uint64_t *) vd;
			return *ui;
		}
		else
			return 0;
	}

	inline double tob_bid() const
	{
		return ((header.bid_num_tiers > 0) ? tier[0].price() : std::numeric_limits<double>::quiet_NaN());
	}

	inline double tob_ask() const
	{
		return ((header.ask_num_tiers > 0) ? tier[header.bid_num_tiers].price() : std::numeric_limits<double>::quiet_NaN());
	}

	inline uint64_t timestamp4burst_handling()
	{
		return header.ts_agg_in;
	}

	inline uint32_t hash() const {
		return (header.client_instrument_id * 1000 + header.ecn);
	}
};
