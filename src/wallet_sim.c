#include "wallet_sim.h"

void       Wallet_Init(Wallet* wallet, Order* pendingOrders, size_t listLength)
{
	wallet->LocalFree = 0.0f;
	wallet->ForeignFree = 0.0f;
	wallet->Fees = 0.0f;

	BufferedList_Init(&wallet->Pending, pendingOrders, sizeof(*pendingOrders), listLength);

}

void       Wallet_SetFees(Wallet* wallet, float fees)
{
	wallet->Fees = fees;
}

void       Wallet_AddLocal(Wallet* wallet, float amount)
{
	wallet->LocalFree += amount;
}

void       Wallet_AddForeign(Wallet* wallet, float amount)
{
	wallet->ForeignFree += amount;
}

bool       Wallet_PlaceOrder(Wallet* wallet, OrderType type, float amount, float price)
{
	switch (type)
	{
	case WALLET_SELL:
		if (wallet->ForeignFree > amount)
		{
			Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
			if (!newOrder)
				return false;

			newOrder->Amount = amount;
			newOrder->Type = type;
			newOrder->Price = price;

			return true;
		}
		break;

	case WALLET_BUY:
		if (wallet->LocalFree > (amount * price))
		{
			Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
			if (!newOrder)
				return false;

			newOrder->Amount = amount;
			newOrder->Type = type;
			newOrder->Price = price;

			return true;
		}
	}

	return false;
}

void       Wallet_ExecuteOrders(Wallet* wallet, float currentPrice)
{
	// TODO: Execute orders
}

Order* Wallet_GetPendingOrders(Wallet* wallet)
{
	return BufferedList_First(&wallet->Pending);
}

Balance      Wallet_GetBalance(Wallet* wallet)
{
	Balance balance;

	// TODO: calculate balance

	return balance;
}
