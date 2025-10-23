#ifndef _WALLET_SIM_H_
#define _WALLET_SIM_H_

#include "bufferedlist.h"

#include <time.h>

typedef struct _Balance_
{
	float Local;
	float Foreign;
} Balance;

typedef struct _Wallet_
{
	Balance      Free;
	Balance      BuyTotal;
	Balance      SellTotal;
	float        Fees;
	BufferedList Pending;
} Wallet;

typedef enum _OrderType_
{
	WALLET_SELL,
	WALLET_BUY,
} OrderType;

typedef struct _Order_
{
	Node      node;
	time_t    TimePlaced;
	OrderType Type;
	float     Price;
	float     Amount;
} Order;

void    Wallet_Init(Wallet* wallet, Order* pendingOrders, size_t listLength);
void    Wallet_Reset(Wallet* wallet);
void    Wallet_SetFees(Wallet* wallet, float fees);
void    Wallet_AddLocal(Wallet* wallet, float amount);
void    Wallet_AddForeign(Wallet* wallet, float amount);
void    Wallet_CancelOrder(Wallet* wallet, Order* order);
bool    Wallet_PlaceOrder(Wallet* wallet, time_t time, OrderType type, float amount, float price);
void    Wallet_ExecuteOrders(Wallet* wallet, float low, float high);
Order*  Wallet_GetPendingOrders(Wallet* wallet);
Balance Wallet_GetBalanceFree(Wallet* wallet);
Balance Wallet_GetBalancePending(Wallet* wallet);
Balance Wallet_TotalSells(Wallet* wallet);
Balance Wallet_TotalBuys(Wallet* wallet);
void    Wallet_Cashout(Wallet* wallet, float price);

#endif
