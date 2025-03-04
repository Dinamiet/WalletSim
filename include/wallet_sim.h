#ifndef _WALLET_SIM_H_
#define _WALLET_SIM_H_

#include "bufferedlist.h"

typedef struct _Wallet_
{
	float        LocalFree;
	float        ForeignFree;
	float        Fees;
	BufferedList Pending;
	float        RealizedProfits;
	float        PendingProfits;
	float        TotalCapital;
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

void   Wallet_Init(Wallet* wallet, Order* pendingOrders, size_t listLength);
void   Wallet_SetFees(Wallet* wallet, float fees);
void   Wallet_AddLocal(Wallet* wallet, float amount);
void   Wallet_AddForeign(Wallet* wallet, float amount);
void   Wallet_CancleOrder(Wallet* wallet, Order* order);
bool   Wallet_PlaceOrder(Wallet* wallet, OrderType type, float amount, float price);
void   Wallet_ExecuteOrders(Wallet* wallet, float currentPrice);
Order* Wallet_GetPendingOrders(Wallet* wallet);
float  Wallet_GetLocalFree(Wallet* wallet);
float  Wallet_GetForeignFree(Wallet* wallet);
float  Wallet_GetPendingValue(Wallet* wallet);
float  Wallet_RealizedProfit(Wallet* wallet);
float  Wallet_PendingProfit(Wallet* wallet);
float  Wallet_TotalCapitalUsed(Wallet* wallet);
float  Wallet_Efficiency(Wallet* wallet);

#endif
