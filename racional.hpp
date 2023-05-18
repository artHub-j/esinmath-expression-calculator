#ifndef _RACIONAL_HPP
#define _RACIONAL_HPP
#include <esin/error>
#include <esin/util>

using std::string;

class racional {
public:

  // Constructora. Construeix un racional en la seva versió simplificada.
  // Es produeix un error si el denominador és 0.
  explicit racional(int n=0, int d=1) throw(error);

  // Constructora per còpia, assignació i destructora.
  racional(const racional & r) throw(error);
  racional & operator=(const racional & r) throw(error);
  ~racional() throw();

  // Consultores. La part_entera d'un racional pot ser
  // positiva o negativa. El residu SEMPRE és un racional positiu.
  int num() const throw();
  int denom() const throw();
  int part_entera() const throw();
  racional residu() const throw();

  /* Sobrecàrrega d'operadors aritmètics. Retorna un racional en la seva
     versió simplificada amb el resultat de l'operació. Es produeix un
     error al dividir dos racionals si el segon és 0.*/
  racional operator+(const racional & r) const throw(error);
  racional operator-(const racional & r) const throw(error);
  racional operator*(const racional & r) const throw(error);
  racional operator/(const racional & r) const throw(error);

  /* Sobrecàrrega de operadors de comparació. Retornen cert, si i només si
     el racional sobre el que s'aplica el mètode és igual (==), diferent
     (!=), menor (<), menor o igual (<=), major (>) o major o igual(>=)
     que el racional r.*/
  bool operator==(const racional & r) const throw();
  bool operator!=(const racional & r) const throw();
  bool operator<(const racional & r) const throw();
  bool operator<=(const racional & r) const throw();
  bool operator>(const racional & r) const throw();
  bool operator>=(const racional & r) const throw();

  // Gestió d'errors.
  static const int DenominadorZero = 21;

private:
  #include "racional.rep"
};
#endif
