#ifndef _TOKEN_HPP
#define _TOKEN_HPP
#include <string>
#include <esin/error>
#include <esin/util>
#include "racional.hpp"

class token {
public:

  enum codi {NULLTOK,
             CT_ENTERA, CT_RACIONAL, CT_REAL, CT_E,
             VARIABLE, VAR_PERCENTATGE,
             SUMA, RESTA, MULTIPLICACIO, DIVISIO, EXPONENCIACIO,
             CANVI_DE_SIGNE, SIGNE_POSITIU,
             SQRT, EXP, LOG, EVALF,
             OBRIR_PAR, TANCAR_PAR, COMA,
             ASSIGNACIO, DESASSIGNACIO, BYEBYE};

/*Constructores: Construeixen tokens pels operadors, les constants enteres,
  les constants racionals, les constants reals i les variables(el seu
  identificador), respectivament.

  La primera constructora s'utiliza para aquells tokens que són operadors,
  funcions predefinides, noms de comandes o símbols de ``puntuació''; per
  tant es produeix un error si el codi és CT_ENTERA, CT_RACIONAL, CT_REAL o
  VARIABLE.

  L'última constructora torna un error si l'string donat no és un
  identificador vàlid per una variable: ha d'estar format exclusivament per
  lletres majúscules, minúscules o caràcters de subratllat _. Dit d'una altra
  forma, només conté caràcters els codis ASCII dels quals estan entre 65
  ('A') i 90('Z'), entre 97 ('a') i 122 ('z') o el 95 ('_') i no pot
  coincidir amb un nom reservat: unassign, e, sqrt, log, exp i evalf.*/
  explicit token(codi cod = NULLTOK) throw(error);
  explicit token(int n) throw(error);
  explicit token(const racional & r) throw(error);
  explicit token(double x) throw(error);
  explicit token(const string & var_name) throw(error);

  // Constructora por còpia, assignació i destructora.
  token(const token & t) throw(error);
  token & operator=(const token & t) throw(error);
  ~token() throw();

/*Consultores: Retornen respectivament el codi i el valor (en el cas de
  constants enteres, racionals o reals) o l'identificador (en el cas de
  variables). Es produeix un error si apliquem una consultora inadequada
  sobre un token, p.e. si apliquem valor_enter sobre un token que no sigui
  una CT_ENTERA.*/
  codi tipus() const throw();
  int valor_enter() const throw(error);
  racional valor_racional() const throw(error);
  double valor_real() const throw(error);
  string identificador_variable() const throw(error);

/*Igualtat i desigualtat entre tokens. Dos tokens es consideren iguals si els
  seus codis ho són i si 1) en cas de ser CT_ENTERA, CT_RACIONAL o CT_REAL,
  els seus valors són iguals i 2) en cas de ser VARIABLE, tenen el mateix
  nom. */
  bool operator==(const token & t) const throw();
  bool operator!=(const token & t) const throw();

/*Precedència entre tokens. L'operador > retorna cert si i només si el token
  és un operador amb major precedència que l'operador del token t. Si algun
  dels tokens no és un operador es produeix un error.*/
  bool operator>(const token & t) const throw(error);
  bool operator<(const token & t) const throw(error);

  // Gestió d'errors.
  static const int IdentificadorIncorrecte      = 11;
  static const int ConstructoraInadequada       = 12;
  static const int ConsultoraInadequada         = 13;
  static const int PrecedenciaEntreNoOperadors  = 14;

private:
  #include "token.rep"
};
#endif
