#include "racional.hpp"

#include <iostream>
#include <cmath>

using namespace std;

//-----------Metodes privats o adicionals:

/*  Funció per calcular el MCD dels enters a i b. 
    COST: O(n), tenint que n és el nombre de vegades que divideixes a entre b.   */
int MCD (int a, int b) 
//PRE: -
//POST: retorna el mcd entre a i b.
{
    int mcd = 1;
    if (a > 0 and b > 0) {
        if (b > a) {   
            int temp = b;
            b = a;
            a = temp;
        }
        while (b != 0) {
			mcd = b;
			b = a % b;
			a = mcd;
        }
    }

    return mcd;
}

/*  Funció que imprimeix el racional r. 
    COST: O(1).  */
void print(const racional &r)
//PRE: -
//POST: Imprimeix el racional r.
{
    cout << r.num() << "/" << r.denom();
}

/*  Funció que retorna el valor real del racional r (r._num/r._den).
    COST: O(1).  */
double racional::valor_decimal(const racional &r)
//PRE: -
//POST: Retorna el valor real del racional r (r._num/r._den).
{
    return double(r._num) / double(r._den);
}

//-----------Metodes publics:

/*  Constructora. Construeix un racional en la seva versió simplificada. 
    Es produeix un error si el denominador és 0. 
    COST: O(1) + O(x) tenint que O(x) és el cost de la crida a la funció MCD().   */
racional::racional(int n, int d) throw(error)
{
	if  (d == 0)  
        throw (error (DenominadorZero));
    else {
        int mcd = MCD(std::abs(n), std::abs(d));
        if (d < 0) {
            n *= -1;
            d *= -1;
        }
    	_num = n / mcd;
		_den = d / mcd;
	}    
}

/*  Constructora per còpia.   
    COST: O(1).  */ 
racional::racional(const racional &r) throw(error)
{
    (*this) = r;
}

/*  Assignació.   
    COST: O(1).   */
racional &racional::operator=(const racional &r) throw(error)
{
    _num = r._num;
    _den = r._den;
    return *this;
}

/*  Destructora.    
    COST: O(0).  */
racional::~racional() throw()
{
}

/* Consultores. La part_entera d'un racional pot ser
 positiva o negativa. El residu SEMPRE és un racional positiu.  
    COST: O(1). */
int racional::num() const throw()
{
    return _num;
}

/*  COST: O(1).  */
int racional::denom() const throw()
{
    return _den;
}

/*  COST: O(1). */
int racional::part_entera() const throw()
{
    int res = 0;
    if (_num < 0) {
        if ((_num / _den) == 0) {
            res = -1;
        }
        else {
            if ((_num % _den) != 0)
                res = -1 + (_num / _den);
            else {
                res = _num / _den;\
            }
        }
    }
    else {
        res = _num / _den;
    }
    return res;
}

/*  COST: O(1) + O(1) + O(4) = O(6).    */
racional racional::residu() const throw()
{
    racional res(this->part_entera());
    res = *this - res;
    return res;
}

/*  Sobrecàrrega d'operadors aritmètics. Retorna un racional en la seva
    versió simplificada amb el resultat de l'operació. Es produeix un
    error al dividir dos racionals si el segon és 0.*/
/*  COST: O(1) + O(x) + O(1) + O(1) + O(1) = O(4) + O(x).  */
racional racional::operator+(const racional &r) const throw(error)
{
    racional res;
    if (_den == r._den)
        res = racional (_num + r._num, _den);
    else {
        res = racional(_num * r._den + r._num * _den, _den * r._den);
    }
    
    if (res.num() == 0)
        res = racional(0, 1);
    
    return res;
}

/*  COST: O(1) + O(x) + O(1) + O(1) + O(1) + O(x) = O(4) + 2*O(x).  */
racional racional::operator-(const racional &r) const throw(error)
{
    racional res;
    if (_den == r._den)
        res = racional (_num - r._num, _den);
    else 
        res = racional(_num * r._den - r._num * _den, _den * r._den);

    if (res.num() == 0)
        res = racional(0, 1);
    
    return res;
}

/*  COST: O(1) + O(x) + O(1) + O(x) = O(2) + 2*O(x).   */
racional racional::operator*(const racional &r) const throw(error)
{
    racional res = racional(_num * r._num, _den * r._den);
    if (res.num() == 0)
        res = racional(0, 1);
    
    return res;
}

/*  COST: O(1) + O(x).*/
racional racional::operator/(const racional &r) const throw(error)
{
	return racional(_num * r._den, _den * r._num);
}

/* Sobrecàrrega de operadors de comparació. Retornen cert, si i només si
    el racional sobre el que s'aplica el mètode és igual (==), diferent
    (!=), menor (<), menor o igual (<=), major (>) o major o igual(>=)
    que el racional r.  */
/*  COST: O(1).  */
bool racional::operator==(const racional &r) const throw()
{
	return (_num == r._num and _den == r._den);
}

/*  COST: O(1).  */
bool racional::operator!=(const racional &r) const throw()
{
	return !(*this == r);
}

/*  COST: O(1).  */
bool racional::operator<(const racional &r) const throw()
{
	return (valor_decimal(*this) < valor_decimal(r));
}

/*  COST: O(1).  */
bool racional::operator<=(const racional &r) const throw()
{
	return (valor_decimal(*this) <= valor_decimal(r));
}	

/*  COST: O(1).  */
bool racional::operator>(const racional &r) const throw()
{
	return !(*this <= r); 
}

/*  COST: O(1).  */
bool racional::operator>=(const racional &r) const throw()
{
	return !(*this < r);
}