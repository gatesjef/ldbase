// BigInt.cpp

#include "Precompile.h"
#include "BigInt.h"
#include <algorithm>


BigInt::BigInt()
{
	memset(this, 0x00, sizeof(BigInt));
}

BigInt::BigInt(int x) 
{
	*this = BigIntSet(x);
}

char ToChar(const BigInt &x)
{
	XAssert(x < 10);
	XAssert(x >= 0);
	return (char)('0' + x.digits[0]);
}

void BigIntSetBit(BigInt &num, int bit, int value)
{
	int index = bit / 32;
	int slot = bit % 32;
	uint32 &digit = num.digits[index];

	digit  &= ~(1 << slot);
	digit  |= value << slot;
}

int BigIntGetBit(const BigInt &num, int bit)
{
	int index = bit / 32;
	int slot = bit % 32;
	const uint32 &digit = num.digits[index];

	if (digit & (1 << slot))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void BigIntDivRem(const BigInt& number, const BigInt& divisor, BigInt &quotient, BigInt &remainder)
{
	PROFILER_FUNC();

	XAssert(divisor != 0);
	quotient = 0;
	remainder = 0;

	for(int i = BigInt::SIZE * 32 - 1; i >= 0; --i)
	{
		remainder = remainder << 1;
		int ithBitOfNumber = BigIntGetBit(number, i);
		BigIntSetBit(remainder, 0, ithBitOfNumber);
		if (remainder >= divisor) 
		{
			remainder -= divisor;
			BigIntSetBit(quotient, i, 1);
		}
	}
}

BigInt operator *(const BigInt& a, const BigInt& b)
{
	BigInt retval = 0;
	for(int i = 0; i < BigInt::SIZE * 32; ++i)
	{
		if (BigIntGetBit(b, i))
		{
			retval += a << i;
		}
	}
	return retval;
}

void operator *=(BigInt& a, const BigInt& b)
{
	a = a * b;
}

BigInt BigIntPow(const BigInt& base, const BigInt& exp)
{
	XAssert(exp < 10000);

	BigInt retval = 1;
	for(int i = 0; i < exp; ++i)
	{
		retval *= base;
	}

	return retval;
}

BigInt operator /(const BigInt& number, const BigInt& divisor)
{
	BigInt quotient = 0;
	BigInt remainder = 0;
	BigIntDivRem(number, divisor, quotient, remainder);
	return quotient;
}

void operator /=(BigInt& number, const BigInt& divisor)
{
	number = number / divisor;
}

BigInt operator %(const BigInt& number, const BigInt& divisor)
{
	BigInt quotient = 0;
	BigInt remainder = 0;
	BigIntDivRem(number, divisor, quotient, remainder);
	return remainder;
}

void operator %=(BigInt& number, const BigInt& divisor)
{
	number = number % divisor;
}

int toInt(const BigInt& bint)
{
	XAssert(bint < 0x7FFFFFFF);
	return (int)bint.digits[0];
}

BigInt operator >>(const BigInt& number, const BigInt& shiftB)
{
	BigInt retval = number;
	for(int i = 0; i < shiftB; ++i)
	{
		retval = retval/2;
	}

	return retval;
}

BigInt operator <<(const BigInt& number, const BigInt& shiftB)
{
	int shift = toInt(shiftB);
	int digitsShift = shift/32;
	int bitShift = shift%32;

	BigInt retval = number;

	// shift digits
	if (digitsShift > 0)
	{
		for(int i = BigInt::SIZE - 1; i >= 0; --i)
		{
			int sourceDigit = i - digitsShift;
			if (sourceDigit >= 0)
			{
				retval.digits[i] = number.digits[sourceDigit];
			}
			else
			{
				retval.digits[i] = 0;
			}
		}
	}
	else
	{
		retval = number;
	}


	// shift bits
	for(int i = BigInt::SIZE - 1; i >= 0; --i)
	{
		retval.digits[i] = retval.digits[i] << bitShift;

		if (i-1 >= 0 && bitShift > 0)
		{
			uint32 digit = retval.digits[i-1] >> (32-bitShift);
			//uint32 base = (1 << (32-bitShift));
			//uint32 digit = retval.digits[i-1] / base;
			retval.digits[i] |= digit;
		}
	}

	return retval;
}

XString BigIntToString_old(const BigInt &x) 
{
	XString retval;
	BigInt rest = x;
	BigInt chr;
	BigInt quotient;

	while(1)
	{
		BigIntDivRem(rest, 10, quotient, chr);
		rest = quotient;

		char c = ToChar(chr);
		retval = XString(&c, 1) + retval;

		if (rest == 0)
		{
			break;
		}
	}

	return retval;
}

XString BigIntToString(const BigInt &x)
{
	if (x == 0) return "0";

	const int MAX_BIGINT_CHARS = 10 * BigInt::SIZE; // this is a little bigger than it needs to be
	char buffer[MAX_BIGINT_CHARS + 1];
	BigInt rest = x;

	// Optimization: 9 digits fit in 32 bits, so we get 9 digits each pass to reduce the number of calls to BigIntDivRem
	const int digitsPerPass = 9;
	const BigInt divisor = 1000000000;
	BigInt remainder;
	BigInt quotient;

	int i;
	for (i = 0; i < MAX_BIGINT_CHARS; )
	{
		BigIntDivRem(rest, divisor, quotient, remainder);
		rest = quotient;

		int passDigits = remainder.digits[0];

		for (int passIndex = 0; passIndex < digitsPerPass; passIndex++)
		{
			int aDigit = passDigits % 10;

			if (rest != 0 || passDigits > 0)
			{
				buffer[i++] = '0' + (char)aDigit;
			}

			passDigits /= 10;
		}

		if (rest == 0)
		{
			break;
		}
	}

	// overflow character buffer?
	XAssert(i < MAX_BIGINT_CHARS);

	buffer[i] = 0;

	std::reverse(buffer, buffer + i);

	return XString(buffer);
}

//struct BigIntD
//{
//	char digits[64];
//};
//
//BigIntD BigIntDMake(uint32 b)
//{
//	BigIntD retval = {};
//
//	int i = 0;
//	while(b > 0)
//	{
//		retval.digits[i] = b % 10;
//		b /= 10;
//		++i;
//	}
//
//	return retval;
//}
//
//void operator+=(BigIntD &a, const BigIntD &b)
//{
//	char carry = 0;
//	for(int i = 0; i < 64; ++i)
//	{
//		char d = a.digits[i] + b.digits[i] + carry;
//		carry = d / 10;
//		d = d % 10;
//		a.digits[i] = d;
//	}
//
//}
//
//void BigIntDShift(BigIntD &addend, int s)
//{
//	for(int i = 63; i >= 0; --i)
//	{
//		if (i-s < 0)
//		{
//			addend.digits[i] = 0;
//		}
//		else
//		{
//			addend.digits[i] = addend.digits[i-s];
//		}
//	}
//}
//
//void operator*=(BigIntD &a, uint32 b)
//{
//	BigIntD total = {};
//	for(int i = 0; i < 64; ++i)
//	{
//		BigIntD addend = BigIntDMake(b);
//		BigIntDShift(addend, i);
//
//		char digitVal = a.digits[i];
//		for(char j = 0; j < digitVal; ++j)
//		{
//			total += addend;
//		}
//	}
//
//	a = total;
//}
//
//BigIntD BigIntDPow(int x, int p)
//{
//	BigIntD d = BigIntDMake(1);
//	for(int i = 0; i < p; ++i)
//	{
//		d *= x;
//	}
//
//	return d;
//}
//
//BigIntD Convert(const BigInt &h)
//{
//	BigIntD total = {};
//
//	for(int i = 0; i < BigInt::SIZE; ++i)
//	{
//		BigIntD temp = BigIntDPow(16, i * 8);
//		temp *= h.digits[i];
//		total += temp;
//	}
//
//	return total;
//}

BigInt BigIntSet(int a)
{
	XAssert(a >= 0);
	BigInt retval = 0;
	retval.digits[0] = (uint32)a;
	return retval;
}

BigInt BigIntSet(uint32 d0, uint32 d1, uint32 d2, uint32 d3, uint32 d4)
{
	BigInt retval;
	retval.digits[0] = d0;
	retval.digits[1] = d1;
	retval.digits[2] = d2;
	retval.digits[3] = d3;
	retval.digits[4] = d4;
	return retval;
}

bool operator==(const BigInt &a, const BigInt &b)
{
	for(int i = 0; i < BigInt::SIZE; ++i)
	{
		if (a.digits[i] != b.digits[i]) return false;
	}

	return true;
}

bool operator!=(const BigInt &a, const BigInt &b)
{
	return !(a == b);
}

bool operator!=(int a, const BigInt &b)
{
	if (a < 0) return true;
	return BigIntSet(a) != b;
}

bool operator!=(const BigInt &b, int a)
{
	if (a < 0) return true;
	return BigIntSet(a) != b;
}

bool operator==(const BigInt &b, int a)
{
	if (a < 0) return false;
	return b == BigIntSet(a);
}

//bool operator<=(const BigInt &a, const BigInt &b)
//{
//	for(int i = 0; i < BigInt::SIZE; ++i)
//	{
//		if (a.digits[i] > b.digits[i]) return false;
//	}
//
//	return true;
//}
//

bool operator>(const BigInt &a, const BigInt &b)
{
	for(int i = BigInt::SIZE-1; i >= 0; --i)
	{
		if (a.digits[i] > b.digits[i]) return true;
		if (a.digits[i] < b.digits[i]) return false;
	}

	return false;
}


//bool operator>(const BigInt &a, const BigInt &b)
//{
//	return !(a<=b);
//}

bool operator<(const BigInt &a, const BigInt &b)
{
	return b>a;
}

bool operator>=(const BigInt &a, const BigInt &b)
{
	return !(a<b);
}

bool operator<=(const BigInt &a, const BigInt &b)
{
	return !(a>b);
}


bool operator>(const BigInt &b, int a)
{
	if (a < 0) return true;
	return b > BigIntSet(a);
}

bool operator<(const BigInt &b, int a)
{
	if (a < 0) return false;
	return b < BigIntSet(a);
}

bool operator<(int a, const BigInt &b)
{
	if (a < 0) return true;
	return BigIntSet(a) < b;
}

BigInt operator+(const BigInt &a, const BigInt &b)
{
	BigInt retval = 0;

	uint32 carry = 0;
	for(int i = 0; i < BigInt::SIZE; ++i)
	{
		uint32 ua = a.digits[i];
		uint32 ub = b.digits[i];
		uint32 uresult = ua+ub;
		uint32 new_carry = uresult < ua;
		uresult += carry;
		carry = new_carry;

		retval.digits[i] = uresult;
	}

	return retval;
}

BigInt operator-(const BigInt &a, const BigInt &b)
{
	BigInt retval = 0;

	uint32 borrow = 0;
	for(int i = 0; i < BigInt::SIZE; ++i)
	{
		uint32 ua = a.digits[i];
		uint32 ub = b.digits[i];

		uint32 uresult;
		
		if ((borrow == 1 && ub == ua) ||
			ub > ua)
		{
			uresult = ua-ub-borrow;
			borrow = 1;
		}
		else
		{
			uresult = ua-ub-borrow;
			borrow = 0;
		}

		retval.digits[i] = uresult;
	}

	if (borrow == 1) {XHalt();}

	return retval;
}

BigInt operator+(const BigInt &a, int b)
{
	BigInt bb = 0;
	bb.digits[0] = b;
	return a+bb;
}

BigInt operator+(int &b, const BigInt &a)
{
	BigInt bb = 0;
	bb.digits[0] = b;
	return a+bb;
}

//BigInt operator*(const BigInt &a, const BigInt &a)
//{
//	for(int i = 0; i < BigInt::Size; ++i)
//}

BigInt operator-(const BigInt &a, int b)
{
	if (b < 0)
	{
		return a+(-b);
	}
	else
	{
		BigInt bb = 0;
		bb.digits[0] = b;
		return a-bb;
	}
}

void operator+=(BigInt &a, const BigInt &b)
{
	a = a + b;
}

void operator+=(BigInt &a, int b)
{
	a = a + b;
}

void operator++(BigInt &a, int)
{
	a += 1;
	//return a;
}

void operator++(BigInt &a)
{
	a += 1;
	//return a;
}

void operator-=(BigInt &a, int b)
{
	a = a - b;
}

void operator-=(BigInt &a, const BigInt &b)
{
	a = a - b;
}
//10^44 planets
