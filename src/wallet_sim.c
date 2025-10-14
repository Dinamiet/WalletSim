#include "wallet_sim.h"

void Wallet_Init(Wallet* wallet, Order* pendingOrders, size_t listLength)
{
	BufferedList_Init(&wallet->Pending, pendingOrders, sizeof(*pendingOrders), listLength);

	Wallet_Reset(wallet);
}

void Wallet_Reset(Wallet* wallet)
{
	wallet->Free.Local        = 0.0f;
	wallet->Free.Foreign      = 0.0f;
	wallet->SellTotal.Foreign = 0.0f;
	wallet->SellTotal.Local   = 0.0f;
	wallet->BuyTotal.Foreign  = 0.0f;
	wallet->BuyTotal.Foreign  = 0.0f;

	BufferedList_Clear(&wallet->Pending);
}

void Wallet_SetFees(Wallet* wallet, float fees) { wallet->Fees = fees / 100.0f; }

void Wallet_AddLocal(Wallet* wallet, float amount) { wallet->Free.Local += amount; }

void Wallet_AddForeign(Wallet* wallet, float amount) { wallet->Free.Foreign += amount; }

void Wallet_CancelOrder(Wallet* wallet, Order* order)
{
	if (!order)
		return;

	switch (order->Type)
	{
		case WALLET_SELL:
			wallet->Free.Foreign += order->Amount;
			break;

		case WALLET_BUY:
			wallet->Free.Local += order->Amount * order->Price;
			break;
	}

	BufferedList_Remove(&wallet->Pending, order);
}

bool Wallet_PlaceOrder(Wallet* wallet, time_t time, OrderType type, float amount, float price)
{
	float cost = amount * price;
	if (cost < 0)
		return false;

	switch (type)
	{
		case WALLET_SELL:
			if (wallet->Free.Foreign > amount)
			{
				Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
				if (!newOrder)
					return false;

				newOrder->TimePlaced = time;
				newOrder->Amount     = amount;
				newOrder->Type       = type;
				newOrder->Price      = price;

				wallet->Free.Foreign -= amount;

				return true;
			}
			break;

		case WALLET_BUY:
			if (wallet->Free.Local > cost)
			{
				Order* newOrder = BufferedList_AddEnd(&wallet->Pending);
				if (!newOrder)
					return false;

				newOrder->TimePlaced = time;
				newOrder->Amount     = amount;
				newOrder->Type       = type;
				newOrder->Price      = price;

				wallet->Free.Local -= cost;

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
		float  fee      = order->Amount * wallet->Fees;
		switch (order->Type)
		{
			case WALLET_SELL:
				if (currentPrice >= order->Price)
				{
					float received = (order->Amount - fee) * order->Price;
					executed       = order;
					wallet->Free.Local += received;
					wallet->SellTotal.Local += received;
					wallet->SellTotal.Foreign += order->Amount - fee;
				}
				break;

			case WALLET_BUY:
				if (currentPrice <= order->Price)
				{
					float received = order->Amount - fee;
					executed       = order;
					wallet->Free.Foreign += received;
					wallet->BuyTotal.Local += received * order->Price;
					wallet->BuyTotal.Foreign += received;
				}
				break;
		}

		allChecked = order == lastOrder;
		order      = BufferedList_Next(order);
		BufferedList_Remove(&wallet->Pending, executed);
	}
}

Order* Wallet_GetPendingOrders(Wallet* wallet) { return BufferedList_First(&wallet->Pending); }

Balance Wallet_GetBalanceFree(Wallet* wallet) { return wallet->Free; }

Balance Wallet_GetBalancePending(Wallet* wallet)
{
	Balance balance;
	balance.Local   = 0.0f;
	balance.Foreign = 0.0f;

	Order* order = Wallet_GetPendingOrders(wallet);
	if (order)
	{
		do {
			balance.Local += order->Price * order->Amount;
			balance.Foreign += order->Amount;

			order = BufferedList_Next(order);
		} while (order != Wallet_GetPendingOrders(wallet));
	}

	return balance;
}

Balance Wallet_TotalSells(Wallet* wallet) { return wallet->SellTotal; }

Balance Wallet_TotalBuys(Wallet* wallet) { return wallet->BuyTotal; }

void Wallet_Cashout(Wallet* wallet, float price)
{
	Order* order = BufferedList_First(&wallet->Pending);

	while (order)
	{
		Wallet_CancelOrder(wallet, order);
		order = BufferedList_First(&wallet->Pending);
	}

	float received = wallet->Free.Foreign * (1.0f - wallet->Fees) * price;
	wallet->Free.Local += received;
	wallet->SellTotal.Local   = received;
	wallet->SellTotal.Foreign = wallet->Free.Foreign * (1.0f - wallet->Fees);
}
