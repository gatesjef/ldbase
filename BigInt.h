// BigInt.h
#pragma once

struct BigInt
{
	BigInt();
	BigInt(int x);

	static const int SIZE = 6;
	uint32 digits[SIZE];
};

extern const BigInt BI0;
extern const BigInt BI1;

//BigInt BigIntSet(int a);
BigInt BigIntSet(uint32 d0, uint32 d1=0, uint32 d2=0, uint32 d3=0, uint32 d4=0);

BigInt operator -(const BigInt& v);

bool operator ==(const BigInt& v1, const BigInt& v2);
bool operator !=(const BigInt& v1, const BigInt& v2);
bool operator <(const BigInt& v1, const BigInt& v2);
bool operator <=(const BigInt& v1, const BigInt& v2);
bool operator >(const BigInt& v1, const BigInt& v2);
bool operator >=(const BigInt& v1, const BigInt& v2);

BigInt operator +(const BigInt& v1, const BigInt& v2);
BigInt operator -(const BigInt& v1, const BigInt& v2);
BigInt operator *(const BigInt& v, const BigInt& f);
BigInt operator /(const BigInt& v, const BigInt& f);
BigInt operator %(const BigInt& v, const BigInt& f);

void operator +=(BigInt& v1, const BigInt& v2);
void operator -=(BigInt& v1, const BigInt& v2);
void operator *=(BigInt& v, const BigInt& f);
void operator /=(BigInt& v, const BigInt& f); 
void operator %=(BigInt& v, const BigInt& f); 

BigInt operator <<(const BigInt& num, const BigInt& shift); 
BigInt operator >>(const BigInt& num, const BigInt& shift); 

void BigIntDivRem(const BigInt& number, const BigInt& divisor, BigInt &quotient, BigInt &remainder);
BigInt BigIntPow(const BigInt &base, const BigInt &exp);

XString BigIntToString(const BigInt &a);





