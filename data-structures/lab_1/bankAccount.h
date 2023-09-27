/*
Robert Owens (rao7utn)
9/07/2020
bankAccount.h
*/

#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

class bankAccount {
	public:
		bankAccount();
		bankAccount(double amount);
		~bankAccount();

		double withdraw(double amount);
		double deposit(double amount);
		double getBalance() const;
	private:
		double balance;
};

#endif