#ifndef _EXPRESSIO_HPP
#define _EXPRESSIO_HPP
#include <list>
#include <string>
#include <esin/error>
#include <esin/util>   
#include "token.hpp"
#include "racional.hpp"

using std::list;
using std::string;

class expressio {
public:

  /* Constructora d'una expressió formada per un sol token: un operand. Si
     s'utiliza el valor del token per defecte es construeix la que
     anomenem "expressió buida". Si el tipus del token no és el del token
     per defecte (NULLTOK), ni el d'una CT_ENTERA, CT_RACIONAL, CT_REAL,
     CT_E, VARIABLE o VAR_PERCENTAtGE es produeix un error sintàctic. */
  expressio(const token t = token()) throw(error);

  /* Constructora a partir d'una seqüència de tokens. Es produeix un error si
     la seqüència és buida o si no es pot construir l'arbre d'expressió
     corresponent(és a dir, si és sintàcticament incorrecta). */
  expressio(const list<token> & l) throw(error);

  // Constructora per còpia, assignació i destructora.
  expressio(const expressio & e) throw(error);
  expressio & operator=(const expressio & e) throw(error);
  ~expressio() throw(error);

  // Retorna cert si i només si s'aplica a l'expressió buida.
  operator bool() const throw();

  /* Operadors d'igualtat i desigualtat. Dues expressions es consideren
     iguals si i només si els seus arbres d'expressió són idèntics. */
  bool operator==(const expressio & e) const throw();
  bool operator!=(const expressio & e) const throw();

  /* Retorna una llista sense repeticions, en qualsevol ordre, amb
     els noms de les variables de l'expressió. */
  void vars(list<string> & l) const throw(error);

  /* Substitueix totes les aparicions de la variable de nom v per
     l'expressió e. Si no existeix la variable v dins de l'expressió a la
     que apliquem aquest mètode l'expressió no es modifica. */
  void apply_substitution(const string & v, const expressio & e) throw(error);

  /* Aplica un pas de simplificació a l'expressió. La subexpressió a
     simplificar es busca seguint el recorregut "left to right" postordre
     explicat a l'apartat "Procés d'avaluació". Es pot produir qualsevol dels
     errors semàntics que apareixen més avall numerats des del 32 al 35. */
  void simplify_one_step() throw(error);

  /* Aplica successius passos de simplificació com l'anterior fins que
     l'expressió es trobi completament simplificada. Llavors diem que es
     troba en "forma normal". Es pot produir qualsevol dels errors
     semàntics que apareixen més avall numerats des del 32 al 35. */
  void simplify() throw(error);

  /* Converteix l'expressió en la seqüència de tokens lt corresponent: els
     operadors apareixen entre els operands si són infixos, abans si són
     prefixos i els símbols de funció van seguits de parèntesis que tanquen
     els seus arguments separats per comes. S'han d'introduir només aquells
     parèntesis que siguin estrictament necessaris per trencar les regles de
     precedència o associativitat en l'ordre d'aplicació dels operadors. */
  void list_of_tokens(list<token> & lt) throw(error);

  // Gestió d'errors.
  static const int ErrorSintactic   = 31;
  static const int NegatElevNoEnter = 32;
  static const int DivPerZero       = 33;
  static const int LogDeNoPositiu   = 34;
  static const int SqrtDeNegatiu    = 35;

private:
  #include "expressio.rep"
};
#endif
