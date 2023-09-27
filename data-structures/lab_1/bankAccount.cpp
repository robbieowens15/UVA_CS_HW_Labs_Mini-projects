/*
Robert Owens (rao7utn)
9/07/2020
bankAccount.cpp
*/

#include "bankAccount.h"

bankAccount::bankAccount(){
	balance = 0.0;
}

bankAccount::bankAccount(double initialBalance):
	balance(initialBalance){}

bankAccount::~bankAccount(){}

double bankAccount::withdraw(double amount){
	//Assume amount >= 0
	double newBalance = balance - amount;
	if (newBalance > 0){
		balance = newBalance;
	}
	return balance;
}

double bankAccount::deposit(double amount){
	//Assume amount >= 0
	balance += amount;
	return balance;
}

double bankAccount::getBalance() const{
	return balance;
}