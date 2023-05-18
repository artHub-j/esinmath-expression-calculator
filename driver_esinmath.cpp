// $Id: driver_esinmath.cpp,v 1.2 2017-09-19 10:54:37+02 esin Exp esin $
// 
// Driver esinmath

#include <string>
#include <vector>
#include <list>
#include <utility>

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <esin/error>
#include <esin/gen_driver>
#include <esin/util>

#include "token.hpp"
#include "racional.hpp"
#include "expressio.hpp"
#include "variables.hpp"
#include "math_sessio.hpp"
#include "math_io.hpp"

using util::nat;
using namespace std;

// Imprimir bool

//  ostream& operator<<(ostream& os, const bool b)
//  {
//    os << (b ? "true" : "false");
//    return os;
//  }

// Imprimir codi

ostream& operator<<(ostream& os, const token::codi& c) {
  switch (c) {
    case token::CT_ENTERA: os << "CT_ENTERA"; break;
    case token::CT_RACIONAL: os << "CT_RACIONAL"; break;
    case token::CT_REAL: os << "CT_REAL"; break;
    case token::CT_E: os << "CT_E"; break;
    case token::VARIABLE: os << "VARIABLE"; break;
    case token::VAR_PERCENTATGE: os << "VAR_PERCENTATGE"; break;
    case token::SUMA: os << "SUMA"; break;
    case token::RESTA: os << "RESTA"; break;
    case token::MULTIPLICACIO: os << "MULTIPLICACIO"; break;
    case token::DIVISIO: os << "DIVISIO"; break;
    case token::EXPONENCIACIO: os << "EXPONENCIACIO"; break;
    case token::CANVI_DE_SIGNE: os << "CANVI_DE_SIGNE"; break;
    case token::SIGNE_POSITIU: os << "SIGNE_POSITIU"; break;
    case token::ASSIGNACIO: os << "ASSIGNACIO"; break;
    case token::DESASSIGNACIO: os << "DESASSIGNACIO"; break;
    case token::SQRT: os << "SQRT"; break;
    case token::EXP: os << "EXP"; break;
    case token::LOG: os << "LOG"; break;
    case token::EVALF: os << "EVALF"; break;
    case token::OBRIR_PAR: os << "OBRIR_PAR"; break;
    case token::TANCAR_PAR: os << "TANCAR_PAR"; break;
    case token::COMA: os << "COMA"; break;
    case token::BYEBYE: os << "BYEBYE"; break;
    default: os << "NULLTOK"; break;
  }
  
  return os;
}

// Imprimir racional

ostream& operator<<(ostream& os, const racional& r)
{
  os << r.num() << "/" << r.denom();
  return os;
}


// Imprimir token

ostream& operator<<(ostream& os, const token& t) { 
  switch (t.tipus()) {
    case token::CT_ENTERA: os << t.valor_enter(); break;
    case token::CT_RACIONAL: os << t.valor_racional(); break;
    case token::CT_REAL: { printf("%f", t.valor_real());  
			    break;
			  }
    case token::CT_E: os << "e"; break;
    case token::VARIABLE: os << t.identificador_variable(); break;
    case token::VAR_PERCENTATGE: os << "%"; break;
    case token::SUMA: os << "+"; break;
    case token::RESTA: os << "-"; break;
    case token::MULTIPLICACIO: os << "*"; break;
    case token::DIVISIO: os << "/"; break;
    case token::EXPONENCIACIO: os << "^"; break;
    case token::CANVI_DE_SIGNE: os << "-"; break;
    case token::SIGNE_POSITIU: os << "+"; break;
    case token::ASSIGNACIO: os << ":="; break;
    case token::DESASSIGNACIO: os << "unassign"; break;
    case token::SQRT: os << "sqrt"; break;
    case token::EXP: os << "exp"; break;
    case token::LOG: os << "log"; break;
    case token::EVALF: os << "evalf"; break;
    case token::OBRIR_PAR: os << "("; break;
    case token::TANCAR_PAR: os << ")"; break;
    case token::COMA: os << ","; break;
    case token::BYEBYE: os << "byebye"; break;
    default: os << "NULLTOK"; break;
  }
  return os; 
}


bool checkNumero(const string &s) {
  bool res = false;
  if (s.length() > 0) {
    unsigned int i = 0;
    bool esnum = true;
    if ((s[0] == '+') || (s[0] == '-')) {
      i++;
    }

    int aux = s.find('.');
    if (aux >= 0) {
      esnum = ((int) s.find('.', aux+1)) < 0;
    }

    while (i < s.length() && esnum) {
      esnum = (s[i] >= '0' && s[i] <= '9') || (s[i] == '.');
      i++;
    }
    res = esnum && (i == s.length());  
  }
  return res;
}

// Crea un token

token* crea_token(gen_driver& dr) {
  token* tk;
  string s = dr.args(3);

  if ("CT_ENTERA" == s) 
    tk = new token(token::CT_ENTERA);
  else if ("CT_RACIONAL" == s)
    tk = new token(token::CT_RACIONAL);
  else if ("CT_REAL" == s)
    tk = new token(token::CT_REAL);
  else if ("CT_E" == s)
    tk = new token(token::CT_E);
  else if ("VARIABLE" == s)
    tk = new token(token::VARIABLE);
  else if ("VAR_PERCENTATGE" == s)
    tk = new token(token::VAR_PERCENTATGE);
  else if ("SUMA" == s)
    tk = new token(token::SUMA);
  else if ("RESTA" == s)
    tk = new token(token::RESTA);
  else if ("MULTIPLICACIO" == s)
    tk = new token(token::MULTIPLICACIO);
  else if ("DIVISIO" == s)
    tk = new token(token::DIVISIO);
  else if ("EXPONENCIACIO" == s)
    tk = new token(token::EXPONENCIACIO);
  else if ("CANVI_DE_SIGNE" == s)
    tk = new token(token::CANVI_DE_SIGNE);
  else if ("SIGNE_POSITIU" == s)
    tk = new token(token::SIGNE_POSITIU);
  else if ("ASSIGNACIO" == s)
    tk = new token(token::ASSIGNACIO);
  else if ("DESASSIGNACIO" == s)
    tk = new token(token::DESASSIGNACIO);
  else if ("SQRT" == s)
    tk = new token(token::SQRT);
  else if ("EXP" == s)
    tk = new token(token::EXP);
  else if ("LOG" == s)
    tk = new token(token::LOG);
  else if ("EVALF" == s)
    tk = new token(token::EVALF);
  else if ("OBRIR_PAR" == s)
    tk = new token(token::OBRIR_PAR);
  else if ("TANCAR_PAR" == s)
    tk = new token(token::TANCAR_PAR);
  else if ("COMA" == s)
    tk = new token(token::COMA);
  else if ("BYEBYE" == s)
    tk = new token(token::BYEBYE);
  else if ("NULLTOK" == s)
    tk = new token(token::NULLTOK);
  else {
    if (checkNumero(s)) {
      if (util::is_int(s)) {
        tk = new token(util::toint(s));
      }
      else {
        tk = new token(util::todouble(s));
      }
    }
    else {
      try {
        racional* r = dr.object<racional>(3);
        tk = new token(*r);
      }
      catch (error e) {
        tk = new token(s);
      }
    }
  }
  
  return tk;
}

// imprimir list<T>

template<typename T>
void imprimeix_list(const list<T>& L,
		  const char sep = ' ',
		  ostream& os = std::cout)
{
  if (!L.empty()) {
    typename list<T>::const_iterator it = L.begin();

    for (nat i = 0; i != L.size()-1; i++) {
      os << *it << sep;
      ++it;
    }
    os << *it;
  }
  os << endl;
}


/* TypeTraits */
template <> const char* TypeTraits<token>::name = "token";
template <> const char* TypeTraits<racional>::name = "racional";
template <> const char* TypeTraits<expressio>::name = "expressio";
template <> const char* TypeTraits<variables>::name = "variables";
template <> const char* TypeTraits<math_sessio>::name = "math_sessio";

template <> const char* TypeTraits<list<token> >::name = "list<token>";
template <> const char* TypeTraits<list<string> >::name = "list<string>";


/* ----------< BASIQUES: Constructores, assignacio i destruccio >---------- */

void* user_init(gen_driver& dr) {
  string id = dr.args(1);
  string tid = dr.args(2);
  
  if (tid == "token") {
    if (dr.nargs() != 3 && dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod, 
		  gen_driver::WrongNumArgsMsg);
    token* t;
    if (dr.nargs() == 3){
      t = crea_token(dr);
    }
    else{
      t = new token;
    }
    return static_cast<void*> (t);
  }

  else if (tid == "racional") { 
    if (dr.nargs() < 2 || dr.nargs() > 4)
      throw error(gen_driver::WrongNumArgs,gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    
    racional* r;
    if (dr.nargs() == 2)
      r = new racional();
    else if (dr.nargs() == 3) {
      int num = util::toint(dr.args(3));
      r = new racional(num);
    }
    else {
      int num = util::toint(dr.args(3)),
	den = util::toint(dr.args(4));
      
      r = new racional(num, den);
    }
    return static_cast<void*> (r);
  }

  else if (tid == "expressio") {
    expressio* e = NULL;
    if (dr.nargs() != 3 && dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod, 
		  gen_driver::WrongNumArgsMsg);
    if (dr.nargs() == 3){
       if (dr.object_type(3) == "token") {
         token* tok = dr.object<token>(3);
         e = new expressio(*tok);
       }
       else if (dr.object_type(3) == "list<token>") {
         list<token>* LT = dr.object<list<token> >(3);
         e = new expressio(*LT);
       }
    }
    else
    {
      e= new expressio;
    }
    return static_cast<void*> (e);
  }

  else if (tid == "variables") {
    if (dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    
    variables* v = new variables();
    return static_cast<void*> (v);
  }
  
  else if (tid == "math_sessio") {
    if (dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    
    math_sessio* es = new math_sessio();
    return static_cast<void*> (es);
  }
  
  else if (tid == "list<string>") {
    if (dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    return static_cast<void*> (new list<string>);
  }
  
  else if (tid == "list<token>") {
    if (dr.nargs() != 2)
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    return static_cast<void*> (new list<token>);
  }
  
  throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
	      gen_driver::WrongTypeArgsMsg);
}

/* ----------------------------< OPERATORS >-------------------------------- */

void iguals(gen_driver& dr)
{
  if (dr.object_type()=="token")
  {
      token* tok1 = dr.object<token>();
      token* tok2 = dr.object<token>(1);
      string s = (*tok1 == *tok2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 == *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="expressio")
  {
      expressio* e1 = dr.object<expressio>();
      expressio* e2 = dr.object<expressio>(1);
      string s = (*e1 == *e2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void diferents(gen_driver& dr)
{
  if (dr.object_type()=="token")
  {
      token* tok1 = dr.object<token>();
      token* tok2 = dr.object<token>(1);
      string s = (*tok1 != *tok2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 != *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="expressio")
  {
      expressio* e1 = dr.object<expressio>();
      expressio* e2 = dr.object<expressio>(1);
      string s = (*e1 != *e2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void menor(gen_driver& dr)
{
  if (dr.object_type()=="token")
  {
      token* tok1 = dr.object<token>();
      token* tok2 = dr.object<token>(1);
      string s = (*tok1 < *tok2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 < *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void major(gen_driver& dr)
{
  if (dr.object_type()=="token")
  {
      token* tok1 = dr.object<token>();
      token* tok2 = dr.object<token>(1);
      string s = (*tok1 > *tok2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 > *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void menor_igual(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 <= *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void major_igual(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      string s = (*r1 >= *r2)? "true" : "false";
      dr.get_ostream() << s << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}


void suma(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      dr.get_ostream() << static_cast<racional> (*r1 + *r2) << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void resta(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      dr.get_ostream() << static_cast<racional> (*r1 - *r2) << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void mult(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      dr.get_ostream() << static_cast<racional> (*r1 * *r2) << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

void div(gen_driver& dr)
{
  if (dr.object_type()=="racional")
  {
      racional* r1 = dr.object<racional>();
      racional* r2 = dr.object<racional>(1);
      dr.get_ostream() << static_cast<racional> (*r1 / *r2) << endl;
  }

  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}

/* ----------------------------< TOKEN >----------------------------------- */

/* Test de memoria dinamica */

void crea_token_enter (token*& t)
{ t = new token (3); }

void crea_token_racional (token*& t)
{ t = new token (racional(8192,16384)); }

void crea_token_real (token*& t)
{ t = new token (3.141592); }

void crea_token_var (token*& t)
{ t = new token ("aux"); }

bool test_ctor_token (gen_driver& d)
{ return d.generic_memtest<token> (crea_token_racional, "ctor token"); }

bool test_copyctor_token (gen_driver& d)
{ return d.copyctor_memtest<token> (crea_token_var); }

bool test_asigna_token (gen_driver& d)
{ return d.assgn_memtest<token>(crea_token_enter, crea_token_racional); }


/* Metodes de la classe */

void tipus(gen_driver& dr)
{
  token* t = dr.object<token>();
  dr.get_ostream() << t -> tipus();
  dr.get_ostream() << endl;
}

void valor_enter(gen_driver& dr)
{
  token* t = dr.object<token>();
  dr.get_ostream() << t -> valor_enter();
  dr.get_ostream() << endl;
}

void valor_racional(gen_driver& dr)
{
  token* t = dr.object<token>();
  dr.get_ostream() << t -> valor_racional();
  dr.get_ostream() << endl;
}

void valor_real(gen_driver& dr)
{
  token* t = dr.object<token>();
  dr.get_ostream() << t -> valor_real();
  dr.get_ostream() << endl;
}

void identificador_variable(gen_driver& dr)
{
  token* t = dr.object<token>();
  dr.get_ostream() << t -> identificador_variable();
  dr.get_ostream() << endl;
}

/* ---------------------------< RACIONAL >--------------------------------- */

void num(gen_driver& dr)
{ dr.get_ostream() << dr.object<racional>()->num() << endl; }

void denom(gen_driver& dr)
{ dr.get_ostream() << dr.object<racional>()->denom() << endl; }

void part_entera(gen_driver& dr)
{ dr.get_ostream() << dr.object<racional>()->part_entera() << endl; }

void residu(gen_driver& dr)
{ dr.get_ostream() << dr.object<racional>()->residu() << endl; }


/* -----------------------------< EXPRESSIO >------------------------------ */

/* Test de memoria dinamica */

void crea_expressio_0(expressio*& e) {
  list<token> LT;
  math_io::scan ("3 + a * 5 / 10", LT);
  e = new expressio(LT);
}

void crea_expressio_N(expressio*& e) {
  list<token> LT;
  math_io::scan ("3 + 3 * 5 / 10 - sqrt (9 ^ 2)", LT);
  e = new expressio(LT);
}

void crea_expressio_y_simpl1(expressio*& e) {
  list<token> LT;
  math_io::scan ("( 3 + 3 * log ( 5 ) ) / ( 2 * e * ( sqrt ( 3 ) ) )", LT);
  e = new expressio(LT);
  e->simplify_one_step();
}

void crea_expressio_y_simplify(expressio*& e) {
  list<token> LT;
  math_io::scan ("( 3 + 3 * log ( 5 ) ) / ( 2 * e * ( sqrt ( 3 ) ) )", LT);
  e = new expressio(LT);
  e->simplify();
}

bool test_ctor_expressio_N(gen_driver& d) {
  return d.generic_memtest<expressio>(crea_expressio_N, "ctor N");
}

bool test_simplify_one_step(gen_driver& d) {
  return d.generic_memtest<expressio>(crea_expressio_y_simpl1,
				      "simplify_one_step");
}

bool test_simplify(gen_driver& d) {
  return d.generic_memtest<expressio>(crea_expressio_y_simplify,
				      "simplify");
}

bool test_copyctor_expressio(gen_driver& d) {
  return d.copyctor_memtest<expressio>(crea_expressio_N);
}

bool test_asigna_expressio(gen_driver& d) {
  return d.assgn_memtest<expressio>(crea_expressio_0,
				    crea_expressio_N);
}

/* Metodes de la classe */

void op_bool(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  string s = (*e)? "true" : "false";
  dr.get_ostream() << s << endl;
}

void vars(gen_driver& dr)
{
  list<string>* l = dr.object<list<string> >(1);
  dr.object<expressio>() -> vars(*l);
  l -> sort();
}

void vars_pantalla(gen_driver& dr)
{
  list<string> l;
  dr.object<expressio>() -> vars(l);
  l.sort();
  imprimeix_list(l, ' ',dr.get_ostream());
}

void apply_substitution(gen_driver& dr)
{
  expressio* e1 = dr.object<expressio>();
  string s = dr.args(1);
  expressio* e2 = dr.object<expressio>(2);
  e1 -> apply_substitution(s, *e2);
}

void apply_substitution_pantalla(gen_driver& dr)
{
  expressio* e1 = dr.object<expressio>();
  string s = dr.args(1);
  expressio* e2 = dr.object<expressio>(2);
  e1 -> apply_substitution(s, *e2);
  list<token> LT;
  e1 -> list_of_tokens(LT);
  imprimeix_list(LT, ' ', dr.get_ostream());
}

void simplify_one_step(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  e -> simplify_one_step();
}

void simplify_one_step_pantalla(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  e -> simplify_one_step();
  list<token> LT;
  e -> list_of_tokens(LT);
  imprimeix_list(LT, ' ', dr.get_ostream());
}

void simplify(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  e -> simplify();
}

void simplify_pantalla(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  e -> simplify();
  list<token> LT;
  e -> list_of_tokens(LT);
  imprimeix_list(LT, ' ', dr.get_ostream()); 
}

void list_of_tokens(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  list<token>* LT = dr.object<list<token> >(1);
  e -> list_of_tokens(*LT);
}

void list_of_tokens_pantalla(gen_driver& dr)
{
  expressio* e = dr.object<expressio>();
  list<token> LT;
  e -> list_of_tokens(LT);
  imprimeix_list(LT, ' ',dr.get_ostream());
}

/* -----------------------------< VARIABLES >------------------------------ */

/* Test de memoria dinamica */

void crea_vars_0(variables*& v) {
  v = new variables();
  expressio e;
  v->assign("aux", e);
  v->assign("aux2", e);
}

void crea_otro_vars(variables*& v) {
  v = new variables();
  expressio e;
  v->assign("aux", e);
  v->assign("aux2", e);
  v->assign("x", e);
  v->assign("y", e);
}

bool test_inserta_variables(gen_driver& d) {
  return d.generic_memtest<variables>(crea_otro_vars, "inserta");
}

bool test_copyctor_variables(gen_driver& d) {
  return d.copyctor_memtest<variables>(crea_otro_vars);
}

bool test_asigna_variables(gen_driver& d) {
  return d.assgn_memtest<variables>(crea_vars_0, crea_otro_vars);
}

/* Metodes de la classe */

void assign(gen_driver& dr)
{ dr.object<variables>() -> assign(dr.args(1), *(dr.object<expressio>(2))); }

void unassign(gen_driver& dr)
{ dr.object<variables>() -> unassign(dr.args(1)); }

void valor(gen_driver& dr)
{
  expressio* e = dr.object<expressio>(2);
  *e = dr.object<variables>() -> valor(dr.args(1));
}

void valor_pantalla(gen_driver& dr)
{
  expressio e = dr.object<variables>() -> valor(dr.args(1));
  list<token> LT;
  e.list_of_tokens(LT);
  imprimeix_list(LT, ' ',dr.get_ostream());
}

/* ---------< CAS ESPECIAL: DUMP (MATH_SESSIO I VARIABLES) > ----------- */

void dump(gen_driver& dr)
{
  list<string>* LS = dr.object<list<string> >(1);
  if (dr.object_type() == "variables")
    dr.object<variables>() -> dump(*LS);
  else if(dr.object_type() == "math_sessio")
    dr.object<math_sessio>() -> dump(*LS);
  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
  LS -> sort();
}

void dump_pantalla(gen_driver& dr)
{
  list<string> LS;
  if (dr.object_type() == "variables")
    dr.object<variables>() -> dump(LS);
  else if (dr.object_type() == "math_sessio")
    dr.object<math_sessio>() -> dump(LS);
  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod,
		gen_driver::WrongTypeArgsMsg);
  LS.sort();
  imprimeix_list(LS, ' ', dr.get_ostream());
}

/* ----------------------------< MATH_SESSIO >-------------------------- */

/* Test de memoria dinamica */

void crea_sesion_1(math_sessio*& es)
{
  es = new math_sessio();
  list<token> LT, LT2;
  math_io::scan("a + a", LT);  
  es -> execute(LT, LT2);
  math_io::scan("x := (3 + 3 * log (5)) / (2 * e * (sqrt (3)))", LT);
  es -> execute(LT, LT2);
  math_io::scan("y := x * log (5)", LT);
  es -> execute(LT, LT2);
}  

void crea_sesion_2(math_sessio*& es)
{
  es = new math_sessio();
  list<token> LT, LT2;
  math_io::scan("x := (3 + 3 * log (5)) / (2 * e * (sqrt (3)))", LT);
  es -> execute(LT, LT2);
}

void f_test_execute(math_sessio*& es)
{
  es = new math_sessio();
  list<token> LT, LT2;
  math_io::scan("a:=(3 * 2 + 3.141592 * sqrt (5))/(3*log(log(5)))", LT);
  es -> execute(LT, LT2);
  math_io::scan("b:= a + a + c", LT);
  es -> execute(LT, LT2);
  math_io::scan("y := a + (b + sqrt (5))/(3*log(log(5)))", LT);
  es -> execute(LT, LT2);
  math_io::scan("unassign a", LT);
  es -> execute(LT, LT2);
  math_io::scan("x+y+a+%", LT);
  es -> execute(LT, LT2);
  math_io::scan("byebye", LT);
  es -> execute(LT, LT2);
}

bool test_execute(gen_driver& d) {
  return d.generic_memtest<math_sessio>(f_test_execute, "execute");
}

bool test_copyctor_math_sessio(gen_driver& d) {
  return d.copyctor_memtest<math_sessio>(crea_sesion_1);
}

bool test_asigna_math_sessio(gen_driver& d) {
  return d.assgn_memtest<math_sessio>(crea_sesion_2, crea_sesion_1);
}

/* Metodes de la classe */

void execute(gen_driver& dr)
{
  math_sessio* es = dr.object<math_sessio>();
  list<token>* LT1 = dr.object<list<token> >(1);
  list<token>* LT2 = new list<token>;
  try {
  es -> execute(*LT1, *LT2);
  } catch (error e) {
    delete LT2;
    throw e;
  }
  delete LT2;
}

void execute_pantalla(gen_driver& dr)
{
  math_sessio* es = dr.object<math_sessio>();
  list<token>* LT1 = dr.object<list<token> >(1);
  list<token>* LT2 = new list<token>;
  try {
    es -> execute(*LT1, *LT2);
    imprimeix_list(*LT2, ' ', dr.get_ostream());
  } catch (error e) {
    delete LT2;
    throw e;
  }
  delete LT2;
}

void end_of_sesion(gen_driver& dr)
{
  math_sessio* es = dr.object<math_sessio>();
  string s = (es -> end_of_session())? "true" : "false";
  dr.get_ostream() << s << endl;
}

void apply_all_substitutions(gen_driver& dr)
{
  math_sessio* es = dr.object<math_sessio>();
  expressio* e = dr.object<expressio>(1);
  es -> apply_all_substitutions(*e);
}

void apply_all_substitutions_pantalla(gen_driver& dr)
{
  math_sessio* es = dr.object<math_sessio>();
  expressio* e = dr.object<expressio>(1);
  es -> apply_all_substitutions(*e);
  list<token> LT;
  e -> list_of_tokens(LT);
  imprimeix_list(LT, ' ', dr.get_ostream());
}


/* ----------------------------< MATH_IO >------------------------------ */

void scan(gen_driver& dr)
{
  string s = dr.args(1);
  list<token>* LT = dr.object<list<token> >(2);
  math_io::scan (s, *LT);
}

void scan_pantalla(gen_driver& dr)
{
  string s = dr.args(1);
  list<token> LT;
  math_io::scan (s, LT);
  imprimeix_list(LT, ' ', dr.get_ostream());
}

void tostring(gen_driver& dr)
{
  list<token>* LT = dr.object<list<token> >(1);
  string s = math_io::tostring(*LT);
  dr.get_ostream() << s << endl;
}


/* ----------------------------<afegir a LIST>------------------------------ */

void inserta_lista(gen_driver& dr)
{  

  if (dr.object_type()=="list<token>")
  {
      list<token>* LT = dr.object<list<token> >();
      token* tok = dr.object<token>(1);
      LT -> push_back (*tok);
  }
  
  else
    throw error(gen_driver::WrongTypeArgs, gen_driver::nom_mod, 
		gen_driver::WrongTypeArgsMsg);
}


/* --------------------------------< MAIN >--------------------------------- */

int main()
{
  gen_driver d("math.err", false);

  // OPERATORS

  // ==
  d.add_call("==", iguals, "any", "any");

  // !=
  d.add_call("!=", diferents, "any", "any");

  // <
  d.add_call("<", menor, "any", "any");

  // >
  d.add_call(">", major, "any", "any");

  // <=

  d.add_call("<=", menor_igual, "any", "any");

  // >=
  d.add_call(">=", major_igual, "any", "any");

  // +
  d.add_call("+", suma, "any", "any");

  // -
  d.add_call("-", resta, "any", "any");

  // *
  d.add_call("*", mult, "any", "any");

  // /
  d.add_call("/", div, "any", "any");

  // TOKEN 

  // tipus
  d.add_call("tipus", tipus, "token");
  d.add_call("tp", tipus, "token");
  
  // valor_enter
  d.add_call("valor_enter", valor_enter, "token");
  d.add_call("ve", valor_enter, "token");

  // valor_racional
  d.add_call("valor_racional", valor_racional, "token");
  d.add_call("vra", valor_racional, "token");

  // valor_real
  d.add_call("valor_real", valor_real, "token");
  d.add_call("vre", valor_real, "token");

  // identificador_variable
  d.add_call("identificador_variable", identificador_variable, "token");
  d.add_call("idvar", identificador_variable,"token");

  // RACIONAL

  // num
  d.add_call("num", num, "racional");

  // denom
  d.add_call("denom", denom, "racional");

  // part_entera
  d.add_call("part_entera", part_entera, "racional");
  d.add_call("pent", part_entera, "racional");

  // residu
  d.add_call("residu", residu, "racional");

  // EXPRESSIO

  // operator (bool)
  d.add_call("enula", op_bool, "expressio");
  d.add_call("ebuida", op_bool, "expressio");

  // vars
  d.add_call("vars", vars, "expressio", "list<string>", "vars <list_string>");

  // vars_pantalla
  d.add_call("varsp", vars_pantalla, "expressio");

  // apply_substitution
  d.add_call("apply_substitution", apply_substitution, "expressio",
	     "string expressio", "apply_substitution <string> <expressio>");

  d.add_call("asubs", apply_substitution, "expressio", "string expressio",
	     "asubs <string> <expressio>");

  // apply_substitution_pantalla
  d.add_call("apply_substitution_pantalla", apply_substitution_pantalla,
	     "expressio", "string expressio");
  d.add_call("asubsp", apply_substitution_pantalla, "expressio",
	     "string expressio");

  // simplify_one_step
  d.add_call("simplify_one_step", simplify_one_step, "expressio");
  d.add_call("sos", simplify_one_step, "expressio");

  // simplify_one_step_pantalla
  d.add_call("simplify_one_step_pantalla", simplify_one_step_pantalla,
	     "expressio");
  d.add_call("sosp", simplify_one_step_pantalla, "expressio");

  // simplify
  d.add_call("simplify", simplify, "expressio");
  d.add_call("simp", simplify, "expressio");

  // simplify_pantalla
  d.add_call("simplify_pantalla", simplify_pantalla, "expressio");
  d.add_call("simpp", simplify_pantalla, "expressio");
  
  // list_of_tokens
  d.add_call("list_of_tokens", list_of_tokens, "expressio", "list<token>");
  d.add_call("lot", list_of_tokens, "expressio", "list<token>");

  // list_of_tokens_pantalla
  d.add_call("list_of_tokens_pantalla", list_of_tokens_pantalla, "expressio");
  d.add_call("lotp", list_of_tokens_pantalla, "expressio");
  
  // VARIABLES

  // assign
  d.add_call("assign", assign, "variables", "string expressio");

  // unassign
  d.add_call("unassign", unassign, "variables", "string");

  // valor
  d.add_call("valor", valor, "variables", "string");

  // valor_pantalla
  d.add_call("valorp", valor_pantalla, "variables", "string");

  // VARIABLES & MATH_SESSIO

  // dump
  d.add_call("dump", dump, "any", "list<string>",
	     "dump <list<string> >");

  // dump_pantalla
  d.add_call("dump_pantalla", dump_pantalla, "any");
  d.add_call("dumpp", dump_pantalla, "any");

  // MATH_SESSIO

  // execute
  d.add_call("execute", execute, "math_sessio", "list<token>");
  d.add_call("exec", execute, "math_sessio", "list<token>");
  
  // execute_pantalla
  d.add_call("executep", execute_pantalla, "math_sessio",
	     "list<token>");
  d.add_call("execp", execute_pantalla, "math_sessio",
	     "list<token>");

  // end of sesion
  d.add_call("end_of_sesion", end_of_sesion, "math_sessio");
  d.add_call("eof", end_of_sesion, "math_sessio");

  // apply_all_substitutions
  d.add_call("apply_all_substitutions", apply_all_substitutions,
	     "math_sessio", "expressio");
  d.add_call("aas", apply_all_substitutions, "math_sessio", "expressio");

  // apply_all_substitutions_pantalla
  d.add_call("apply_all_substitutionsp", apply_all_substitutions_pantalla,
	     "math_sessio","expressio");
  d.add_call("aasp", apply_all_substitutions_pantalla, "math_sessio", "expressio");

  // MATH_IO

  // scan
  d.add_call("scan", scan, "*", "string list<token>");

  // scan Pantalla
  d.add_call("scanp", scan_pantalla, "*", "string"); 

  // tostring
  d.add_call("tostring", tostring, "*", "list<token>");

  // add to list<token>
  d.add_call("inserta_lista", inserta_lista, "any", "any");
  d.add_call("il", inserta_lista,"any", "any");

  // instal·lacio de tipus
  d.install_type<token>();
  d.install_type<racional>();
  d.install_type<expressio>();
  d.install_type<variables>();
  d.install_type<math_sessio>();

  d.install_std_type<list<string> >("list<string>");
  d.install_std_type<list<token> >("list<token>");

  d.go();

}
