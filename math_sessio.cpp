#include "math_sessio.hpp"
#include <iostream>

using namespace std;

//-----------Metodes privats o adicionals:

/*	Metode auxiliar per sustituir el token VAR_PERCENTATGE a lin per els tokens de l2.	*/
list<token> math_sessio::sustituir_tokens (list<token> &lin, list<token> &l2) 
//PRE:	True
//POST:	Retorna una llista amb els elements de l1 i l2, substituint el token VAR_PERCENTAGE de l1
//		per el contingut de l2.
{
	list<token> res;
	bool trobat = false;
	list<token>::const_iterator aux;
	for (list<token>::const_iterator it = lin.begin(); it != lin.end() and not trobat; ++it) {
		aux = it;
		trobat = ((*it).tipus() == token::VAR_PERCENTATGE);
		if (trobat)
			break;
		res.push_back((*it));
	}
	++aux;
	if (trobat) {
		for (list<token>::const_iterator it2 = l2.begin(); it2 != l2.end(); ++it2) {
			res.push_back((*it2));
		}
		// si queden elem a lin
		for (list<token>::const_iterator it3 = aux; it3 != lin.end(); ++it3) {
			res.push_back((*it3));
		}
	}

	return res;
}

//-----------Metodes publics:

/* 	Constructora. Crea una nova sessió buida i emmagatzema a la variable
 	especial % l'expressió buida. */
math_sessio::math_sessio() throw(error)
{
	variables c;
	expressio e = expressio();
	c.assign("%", e);
	_dicc = c;
	_estat_sessio = false;
}

// Constructora per còpia, assignació i destructora.
math_sessio::math_sessio(const math_sessio & es) throw(error)
{
	(*this) = es;
}
math_sessio & math_sessio::operator=(const math_sessio & es) throw(error)
{
	_dicc = es._dicc;
	_estat_sessio = es._estat_sessio;
	return *this;
}

math_sessio::~math_sessio() throw(error)
{
}

/* Aquest mètode rep una llista de tokens, lin, lèxicament correcta.
 Primerament analitza parcialment lin per verificar si la comanda és
 correcta. Si és correcta executa la comanda que conté lin.
 Les comandes són:
  * avaluació d'una expressió E.
  * assignació v :=  E. S'avalua E i s'assigna el resultat a la variable
	de nom v.
  * desassignació d'una variable v: unassign v.
  * final de sessió: byebye

 En l'anàlisi de la comanda lin es produeix un error de comanda
 incorrecta en els següents casos:
  * si conté el token DESASSIGNACIO i,
	* o bé la comanda no té dos tokens
	* o bé aquest no és el primer token
	* o bé el segon token no és una VARIABLE.
  * si conté el token BYEBYE i aquest no és el primer i únic token
	de la comanda.
  * si conté el token ASSIGNACIO i,
	* o bé la comanda té longitud menor que dos
	* o bé no és el segon token
	* o bé el primer token no és un token VARIABLE.

 Les comandes que involucren avaluació (avaluar i assignar) retornen
 l'expressió avaluada en forma de llista de tokens en lout. La comanda
 unassign retorna la llista que conté com únic token la variable
 desassignada. Finalment la comanda byebye retorna la llista buida.
 Es produeix un error en una assignació, si després l'avaluació de
 l'expressió es comprova que la variable assignada forma part del conjunt
 de variables de l'expressió avaluada, tal i com s'explica a
 l'apartat "Procés d'avaluació". */
void math_sessio::execute(const list<token> & lin, list<token> & lout) throw(error)
{
	bool incorrecte = true, assig = false, unassig = false, bye = false, var_esp = false;
	bool circ_infinita = false;
	incorrecte = (lin.front().tipus() == token::ASSIGNACIO);

	list<token>::const_iterator aux = lin.begin();
	++aux;
	for (list<token>::const_iterator it = lin.begin(); not incorrecte and it != lin.end(); ++it) {
		if ((*it).tipus() == token::ASSIGNACIO) {
			incorrecte = (lin.front().tipus() != token::VARIABLE) or (lin.size() < 3) or ((*aux).tipus() != token::ASSIGNACIO);
			if (not incorrecte) {
				assig = true;
			}
		}
		else if ((*it).tipus() == token::DESASSIGNACIO) {
			incorrecte = (lin.size() != 2);
			if (not incorrecte)
				incorrecte = (lin.front().tipus() != token::DESASSIGNACIO);
			if (not incorrecte) {
				list<token>::const_iterator it_aux = ++it;
				incorrecte = ((*it_aux).tipus() != token::VARIABLE);
			}
			if (not incorrecte)
				unassig = true;
		}
		else if ((*it).tipus() == token::VAR_PERCENTATGE) { // Variable especial %
				var_esp = true;
		}
		else if ((*it).tipus() == token::BYEBYE) {
			incorrecte = (lin.size() > 1);
			if (not incorrecte)
				bye = true;
		}
	}
	if (incorrecte) {
		throw error(SintComandaIncorrecta);
	}
	else {
		if (assig) {
			/*ASSIGNACIO*/
			list<token> lin_restant, contingut_list_tokens;
			lin_restant	= lin;
			lin_restant.pop_front();
			lin_restant.pop_front();
			
			for (list<token>::const_iterator it = lin_restant.begin(); not circ_infinita and it != lin_restant.end(); ++it) {
				if (lin.front() == (*it))
					circ_infinita = true;
			}

			if (not circ_infinita) {
				list<token> lout_assig;
				expressio expr = expressio(lin_restant);
				apply_all_substitutions(expr);
				expr.simplify();
				_dicc.assign(lin.front().identificador_variable(), expr);
				_dicc.assign("%", expr);
				(_dicc.valor(lin.front().identificador_variable())).list_of_tokens(lout_assig);
				lout = lout_assig;
			}
			else 
				throw error(AssigAmbCirculInfinita);
		}
		else if (unassig) {
			/*DESASSIGNACIO*/
			_dicc.unassign(lin.back().identificador_variable());
			lout.push_back(lin.back());
		}
		else if (var_esp) {
			/*VAR PERCENTATGE*/
			list<token> auxx, auxxx;
			expressio expr_var_p = (_dicc.valor("%"));
			expr_var_p.list_of_tokens(auxx);
		
			if (auxx.front().tipus() == token::NULLTOK) { // % no te cap expressio assignada.
				list<token> lout_null;
				lout_null.push_back(token(token::VAR_PERCENTATGE));
				lout = lout_null;
			}
			if (lin.size() == 1) {
				list<token> lout_var_p;
				apply_all_substitutions(expr_var_p);
				expr_var_p.simplify();
				expr_var_p.list_of_tokens(lout_var_p);
				lout = lout_var_p;
			}
			else if (auxx.front().tipus() == token::NULLTOK) {
				list<token> lout_r;
				expressio r(lin);
				apply_all_substitutions(r);
				r.simplify();
				r.list_of_tokens(lout_r);
				lout = lout_r;
			}
			else {
				list<token> lin2 = lin, lout_res;
				auxxx = sustituir_tokens(lin2, auxx);
				expressio res(auxxx);
				apply_all_substitutions(res);
				res.simplify();
				res.list_of_tokens(lout_res);
				lout = lout_res;
			}
		}
		else if (bye) {
			/*BYE*/
			_estat_sessio = true;
		}
		else {
			list<token> lout_gen;
			expressio e(lin);
			apply_all_substitutions(e);
			e.simplify();
			_dicc.assign("%", e);
			e.list_of_tokens(lout_gen);
			lout = lout_gen;
		}
	}
}

// Retorna cert si i només si la sessió ha finalitzat.
bool math_sessio::end_of_session() const throw()
{
	return _estat_sessio;
}

/* Retorna en forma de llista d'strings, en un ordre qualsevol, la llista de
 variables juntament amb el seu valor assignat. Cada string de la llista
 té el format id = e, on id és el nom d'una variable i e és l'expressió
 (com string) assignada a id. Convertim una expressió e en el seu string
 corresponent invocant al mètode tostring del mòdul esinmath_io. */
void math_sessio::dump(list<string> & l) const throw(error)
{
	list<string> ls;
	_dicc.dump(ls);
	string s;
	for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it) {
		list<token> lt;
		s = (*it);
		s += " = ";
		_dicc.valor((*it)).list_of_tokens(lt);
		s += math_io::tostring(lt);
		l.push_back(s);
	}
}

/* Donada una expressió e, aplica a les seves variables totes les
 substitucions definides per elles. L'expressió resultant només contindrà
 variables per les quals no hi ha substitució definida (no estan en el
 conjunt). Aquest procés s'explica en detall a l'apartat "Procés
 d'avaluació". S'assumeix que no existeix circularitat infinita entre les
 substitucions de les variables que formen part de l'expressió e. */
void math_sessio::apply_all_substitutions(expressio & e) const throw(error)
{
	list<string> ls, lv;
	expressio aux = e;
	e.vars(ls);
	_dicc.dump(lv);
	for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it) {
		for (list<string>::iterator it2 = lv.begin(); it2 != lv.end(); ++it2) {
			if ((*it) == (*it2)) {
				e.apply_substitution((*it), _dicc.valor((*it)));
			}
		}
	}
	if (aux != e) {
		apply_all_substitutions(e);
	}
}
