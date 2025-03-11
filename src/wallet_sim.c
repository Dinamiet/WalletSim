#include "wallet_sim.h"

void Wallet_Init(Wallet* wallet, Order* pendingOrders, size_t listLength)
{
	wallet->LocalFree   = 0.0f;
	wallet->ForeignFree = 0.0f;
	wallet->Fees        = 0.0f;
	wallet->RealizedProfits = 0.0f;
	wallet->PendingProfits  = 0.0f;
	wallet->TotalCapital    = 0.0f;

	BufferedList_Init(&wallet->Pending, pendingOrders, sizeof(*pendingOrders), listLength);
}

void Wallet_Reset(Wallet* wallet)
{
	wallet->LocalFree       = 0.0f;
	wallet->ForeignFree     = 0.0f;
	wallet->RealizedProfits = 0.0f;
	wallet->PendingProfits  = 0.0f;
	wallet->TotalCapital    = 0.0f;

	BufferedList_Clear(&wallet->Pending);
}

void Wallet_SetFees(Wallet* wallet, float fees) { wallet->Fees = fees; }

void Wallet_AddLocal(Wallet* wallet, float amount) { wallet->LocalFree += amount; }

void Wallet_AddForeign(Wallet* wallet, float amount) { wallet->ForeignFree += amount; }

void Wallet_CancelOrder(Wallet* wallet, Order* order)
{
	float cost = order->Amount * order->Price;
	if (!order)
		return;

	switch (order->Type)
	{
		case WALLET_SELL:
			wallet->ForeignFree += order->Amount;
			wallet->PendingProfits -= cost;
			break;

		case WALLET_BUY:
			wallet->LocalFree += order->Amount * order->Price;
			wallet->PendingProfits += cost;
			wallet->TotalCapital -= cost;
			break;
	}

	BufferedList_Remove(&wallet->Pending, order);
}

bool Wallet_PlaceOrder(Wallet* wallet, OrderType type, float amount, float price)
{
	float cost = amount * price;
	if (cost < 0)
		return false;

	switch (type)
	{
		case WALLET_SELL:
			if (wallet->ForeignFree > amount)
			{
				Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
				if (!newOrder)
					return false;

				newOrder->Amount = amount;
				newOrder->Type   = type;
				newOrder->Price  = price;

				wallet->ForeignFree -= amount;
				wallet->PendingProfits += cost;

				return true;
			}
			break;

		case WALLET_BUY:
			if (wallet->LocalFree > cost)
			{
				Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
				if (!newOrder)
					return false;

				newOrder->Amount = amount;
				newOrder->Type   = type;
				newOrder->Price  = price;

				wallet->LocalFree -= cost;
				wallet->PendingProfits -= cost;
				wallet->TotalCapital += cost;

				return true;
			}
			break;
	}

	return false;
}

void Wallet_ExecuteOrders(Wallet* wallet, float currentPrice)
{
	Order* order      = BufferedList_First(&wallet->Pending);
	Order* lastOrder  = BufferedList_Last(&wallet->Pending);
	bool   allChecked = false;

	while (!allChecked && order)
	{
		Order* executed = NULL;
		float  fee      = order->Amount * (wallet->Fees / 100.0f);
		switch (order->Type)
		{
			case WALLET_SELL:
				if (currentPrice >= order->Price)
				{
					float received = (order->Amount - fee) * order->Price;
					executed = order;
					wallet->LocalFree += received;
					wallet->RealizedProfits += received;
					wallet->PendingProfits -= received;
				}
				break;

			case WALLET_BUY:
				if (currentPrice <= order->Price)
				{
					float received = order->Amount - fee;
					executed = order;
					wallet->ForeignFree += received;
					wallet->RealizedProfits -= received * order->Price;
					wallet->PendingProfits += received * order->Price;
				}
				break;
		}

		allChecked = order == lastOrder;
		order      = BufferedList_Next(order);
		BufferedList_Remove(&wallet->Pending, executed);
	}
}

Order* Wallet_GetPendingOrders(Wallet* wallet) { return BufferedList_First(&wallet->Pending); }

float Wallet_GetLocalFree(Wallet* wallet)
{
	return wallet->LocalFree;
}

float Wallet_GetForeignFree(Wallet* wallet)
{
	return wallet->ForeignFree;
}

float Wallet_GetPendingValue(Wallet* wallet)
{
	float  pendingValue = 0.0f;
	Order* order        = Wallet_GetPendingOrders(wallet);
	if (order)
	{
		do {
			pendingValue += order->Price * order->Amount;

			order = BufferedList_Next(order);
		} while (order != Wallet_GetPendingOrders(wallet));
	}

	return pendingValue;
}

float Wallet_RealizedProfit(Wallet* wallet) { return wallet->RealizedProfits; }
float Wallet_PendingProfit(Wallet* wallet) { return wallet->PendingProfits; }
float Wallet_TotalCapitalUsed(Wallet* wallet) { return wallet->TotalCapital; }
float Wallet_Efficiency(Wallet* wallet) { return (wallet->RealizedProfits + wallet->PendingProfits) / wallet->TotalCapital; }
