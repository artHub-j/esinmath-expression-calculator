#ifndef _MATH_IO_HPP
#define _MATH_IO_HPP
#include <list>
#include <string>
#include <esin/error>
#include <esin/util>
#include "token.hpp"  

using std::list;
using std::string;

namespace math_io {

  /* Realitza l'anàlisi lèxic de la comanda en forma d'string s, la converteix
     en la seqüència de tokens i retorna aquesta seqüència. Es produeix un
     error si existeix algun error lèxic, és a dir, algun caràcter que
     impedeixi el reconeixement d'un nou token. */
  void scan(const string & s, list<token> & lt) throw(error);

  // Converteix en un string la seqüència de tokens lt.
  string tostring(const list<token> & lt);

  // Gestió d'errors.
  const int ErrorLexic = 61;
};
#endif
