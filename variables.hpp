#ifndef _VARIABLES_HPP
#define _VARIABLES_HPP
#include <list>
#include <string>
#include <esin/error>
#include <esin/util>   
#include "expressio.hpp"

using std::list;
using std::string;

class variables {
public:

  // Construeix un conjunt de variables buit.
  variables() throw(error);

  // Constructora per còpia, assignació i destructora.
  variables(const variables & v) throw(error);
  variables & operator=(const variables & v) throw(error);
  ~variables() throw(error);

  /* Afegeix al conjunt de variables la variable de nom v juntament amb el seu
     valor-expressió e. Si la variable v ja existia llavors li assigna el nou
     valor-expressió. */
  void assign(const string & v, const expressio & e) throw(error);

  /* Elimina del conjunt de variables el parell amb la variable de nom v. Si
     la variable no existeix llavors no fa res. */
  void unassign(const string & v) throw();

  /* Consulta el valor-expressió de la variable v. Si la variable no està en
     el conjunt de variables retorna l'expressió buida. */
  expressio valor(const string & lv) const throw(error);

  /* Retorna en l totes les claus del conjunt de variables, en un ordre
     qualsevol. Si no hi ha cap clau retorna la llista buida.*/
  void dump(list<string> & l) const throw(error);

private:
  #include "variables.rep"
};
#endif
