#ifndef _WALLET_SIM_H_
#define _WALLET_SIM_H_

#include "bufferedlist.h"

typedef struct _Wallet_
{
	float        LocalFree;
	float        ForeignFree;
	float        Fees;
	BufferedList LocalPending;
	BufferedList ForeignPending;
} Wallet;

typedef enum _OrderType_
{
	WALLET_SELL,
	WALLET_BUY,
} OrderType;

typedef struct _Order_
{
	Node      node;
	OrderType Type;
	float     Price;
	float     Amount;
} Order;

void Wallet_Init(Wallet* wallet, Order* localpendingList, Order* foreignPendingList);
void Wallet_SetFees(Wallet* wallet, float fees);
void Wallet_AddLocal(Wallet* wallet, float amount);
void Wallet_AddForeign(Wallet* wallet, float amount);
bool Wallet_PlaceOrder(Wallet* wallet, OrderType type, float amount, float price);
void Wallet_ExecuteOrders(Wallet* wallet, float currentPrice);
void Wallet_GetPendingOrders(Wallet* wallet);
void Wallet_GetBalance(Wallet* wallet);

#endif
