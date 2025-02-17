

SELL = 0
BUY = 1

class Order:
	def __init__(self, orderType, price, amount):
		self.Type = orderType
		self.Price = price
		self.Amount = amount

	def __repr__(self):
		t = "BUY" if self.Type is BUY else "SELL" if self.Type == SELL else "UNKNOWN"
		return str({'Type': t, 'Price': self.Price, 'Amount': self.Amount})

class Balance:
	def __init__(self, free, reserved):
		self.Free = free
		self.Reserved = reserved

	def __repr__(self):
		return str({'Free': self.Free, 'Reserved': self.Reserved})

class Wallet:
	def __init__(self, localBalance, foreignBalance):
		self.LocalFree = localBalance
		self.ForeignFree = foreignBalance
		self.LocalPending= []
		self.ForeignPending= []

	def PlaceOrder(self, orderType, amount, price):
		order = Order(orderType, price, amount)
		if order.Type == SELL:
			if self.ForeignFree > order.Amount:
				self.ForeignPending.append(order)
				self.ForeignFree -= order.Amount
				return True
		elif order.Type == BUY:
			if self.LocalFree > (order.Amount * order.Price):
				self.LocalPending.append(order)
				self.LocalFree -= order.Amount * order.Price
				return True

		return False


	def ExecuteOrders(self, currentPrice):
		for order in self.ForeignPending:
			if currentPrice >= order.Price:
				self.ForeignPending.remove(order)
				self.LocalFree += order.Amount * order.Price

		for order in self.LocalPending:
			if currentPrice <= order.Price:
				self.LocalPending.remove(order)
				self.ForeignFree += order.Amount

	def	GetPendingOrders(self):
		return self.LocalPending + self.ForeignPending

	def GetBalance(self):
		localPending = sum([order.Amount * order.Price for order in self.LocalPending])
		foreignPending = sum([order.Amount for order in self.ForeignPending])
		local = Balance(self.LocalFree, localPending)
		foreign = Balance(self.ForeignFree, foreignPending)
		return {'Local': local, 'Foreign': foreign}
