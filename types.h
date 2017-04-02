//(C) 2010, 2011, 2012 Chronos Research LLC. All Rights Reserved.
// (C) 2010, 2011, 2012 Chronos Research LLC. All Rights Reserved.

#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <stdint.h>

typedef char ClientOrderId[47]; // love prime numbers (30 should suffice but...)
typedef char OrderId[47];
typedef uint32_t SessionId;
typedef uint32_t SequenceNum;

typedef uint8_t ExchangeId;
typedef uint32_t OrderPriceType;
#define OrderPriceType_Max 0x7fffffff
#define OrderPriceType_Min 0x0
typedef uint32_t OrderSizeType;
typedef uint8_t EnumType;

typedef uint32_t InstrumentId;
typedef uint32_t IPv4Type;
typedef uint16_t PortType;
typedef uint16_t ClientInstrumentId;
typedef uint16_t MaturityDate;

#define LP_TYPE_LEN 3
#define MAX_ECNS 256
#define INVALID_ECN 255

typedef char LpName[8];

enum AdapterManagerMsgType
{
	NEW_ORDER = 1,
	CANCEL_ORDER = 2,
	CANCEL_ALL_ORDERS = 3,
	UMBILICAL_DIRECTIVE = 5,
	CONNECT_ECN = 6,
	DISCONNECT_ECN = 7,
	CONNECT_ALL_ECNS = 8,
	DISCONNECT_ALL_ECNS = 9,
	TEAR_DOWN_AND_RECONNECT = 10,
	UPDATE_TIME_ZONE = 11,
	CLEAR_RATES = 12,
	SEND_QUOTE_CANCEL = 13
};

#endif  // __COMMON_TYPES_H__
