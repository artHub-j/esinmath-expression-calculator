#include "token.hpp"

//-----------Metodes privats o adicionals:
  
/*	Funcio auxiliar per indicar si un string s es un id valid per a un token del tipus VARIABLE:
	Es valid si: s no es una cadena reservada ("unassign", "e", "sqrt", "log", "exp", "evalf").
	I si el codi ascii dels caracters de s estàn comprésos entre 65 i 90 o si estàn comprésos entre 97 i 122 o si són iguals a 95. 
	COST: O(n), sent n el tamany de l'string s.	*/
bool es_id_valid(const string & s)
//PRE: -
//POST: Retorna True sii s és un identificador valid per a un token del tipus VARIABLE.
//		En cas contari, retorna false.
{
	bool valid = not s.empty() and (s != "unassign" and s != "e" and s != "sqrt" and 
					 s != "log" and s !=  "exp" and s != "evalf");

	for (unsigned int i = 0; i < s.size() and valid; ++i) {
		valid = valid and ((s[i] >= 65 and s[i] <= 90) or (s[i] >= 97 and s[i] <= 122) or (s[i] == 95));
	}
	return valid;
}

//-----------Metodes publics:

/*	Constructores: Construeixen tokens pels operadors, les constants enteres,
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
	coincidir amb un nom reservat: unassign, e, sqrt, log, exp i evalf.	*/
/*  COST: O(1).	*/
token::token(codi cod) throw(error)
{
    if (cod == CT_ENTERA or cod == CT_RACIONAL or cod == CT_REAL or cod == VARIABLE) {
        throw error(ConstructoraInadequada);
    }
    else {
		_codi = cod;
	}
}

/*  COST: O(1).	*/
token::token(int n) throw(error)
{
    p_element = new int(n);
    _codi = CT_ENTERA;
}

/*  COST: O(1).	*/
token::token(const racional & r) throw(error)
{
    p_element = new racional(r);
    _codi = CT_RACIONAL;
}

/*  COST: O(1).	*/
token::token(double x) throw(error)
{
    p_element = new double(x);
    _codi = CT_REAL;
}

/*  COST: O(1).	*/
token::token(const string & var_name) throw(error)
{
	if (not es_id_valid(var_name))
		throw error(IdentificadorIncorrecte);
	else {
		p_element = new string(var_name);
		_codi = VARIABLE;
	}
}

/*	Constructora por còpia.
	COST: O(1).	*/
token::token(const token & t) throw(error)
{
    p_element = t.p_element;
	_codi = t._codi;
}

/*  Assignació.
	COST: O(1).	*/
token & token::operator=(const token & t) throw(error)
{
	p_element = t.p_element;
    _codi = t._codi;
    return *this;
}

/*  Destructora.
	COST: O(0).	*/
token::~token() throw()
{
	
}

/*Consultores: Retornen respectivament el codi i el valor (en el cas de
  constants enteres, racionals o reals) o l'identificador (en el cas de
  variables). Es produeix un error si apliquem una consultora inadequada
  sobre un token, p.e. si apliquem valor_enter sobre un token que no sigui
  una CT_ENTERA.*/
/*  COST: O(1).	*/
token::codi token::tipus() const throw()
{
    return _codi;
}

/*  COST: O(1).	*/
int token::valor_enter() const throw(error)
{
	if (_codi != CT_ENTERA)
		throw error(ConsultoraInadequada);
	else {
		return *(int*)p_element;
	}
}

/*  COST: O(1).	*/
racional token::valor_racional() const throw(error)
{
	if (_codi != CT_RACIONAL)
		throw error(ConsultoraInadequada);
	else {
		return *(racional*)p_element;
	}
}

/*  COST: O(1).	*/
double token::valor_real() const throw(error)
{
	if (_codi != CT_REAL)
		throw error(ConsultoraInadequada);
	else {
		return *(double*)p_element;
	}
}

/*  COST: O(1).	*/
string token::identificador_variable() const throw(error)
{
	if (_codi != VARIABLE)
		throw error(ConsultoraInadequada);
	else {
		return *(string*)p_element;
	}
}

/*	Igualtat i desigualtat entre tokens. Dos tokens es consideren iguals si els
	seus codis ho són i si 1) en cas de ser CT_ENTERA, CT_RACIONAL o CT_REAL,
	els seus valors són iguals i 2) en cas de ser VARIABLE, tenen el mateix
	nom.	*/
/*  COST: O(1) + O(1) + O(1) = O(3).	*/
bool token::operator==(const token & t) const throw()
{
	bool iguals = false;
	
	iguals = (_codi == t._codi);
	
	if (iguals) {
		if (_codi == CT_ENTERA) {
			iguals = (this->valor_enter() == t.valor_enter());
		}
		else if (_codi == CT_RACIONAL) {
			iguals = (this->valor_racional() == t.valor_racional());
		}
		else if (_codi == CT_REAL) {
			iguals = (this->valor_real() == t.valor_real());
		}
		else if (_codi == VARIABLE) {
			iguals = (this->identificador_variable() == t.identificador_variable());
		}
	}
	
	return iguals;
}

/*  COST: O(3).	*/
bool token::operator!=(const token & t) const throw()
{
    return !(*this == t);
}

/*Precedència entre tokens. L'operador > retorna cert si i només si el token
  és un operador amb major precedència que l'operador del token t. Si algun
  dels tokens no és un operador es produeix un error.*/
/*  COST: O(1).	*/
bool token::operator>(const token & t) const throw(error)
{   
	bool b = false;
	if ((_codi != EXPONENCIACIO and _codi != CANVI_DE_SIGNE and _codi != SIGNE_POSITIU and 
		 _codi != MULTIPLICACIO and _codi != DIVISIO and _codi != SUMA and _codi != RESTA) or (
		 t._codi != EXPONENCIACIO and t._codi != CANVI_DE_SIGNE and t._codi != SIGNE_POSITIU and 
		 t._codi != MULTIPLICACIO and t._codi != DIVISIO and t._codi != SUMA and t._codi != RESTA))
	{
		throw error(PrecedenciaEntreNoOperadors);
	}
	else {
		if (t._codi == _codi) {
			b = false;
		}
		else if (t._codi == EXPONENCIACIO) {
			b = false;
		}
		else if (_codi == EXPONENCIACIO) {
			b = true;
		}
		// MATEIX NIVELL
		else if ((_codi == CANVI_DE_SIGNE or _codi == SIGNE_POSITIU) and 
				(t._codi == CANVI_DE_SIGNE or t._codi == SIGNE_POSITIU)) { 
			b = false;
		}
		else if ((_codi == MULTIPLICACIO or _codi == DIVISIO) and 
				(t._codi == MULTIPLICACIO or t._codi == DIVISIO)) { 
			b = false;
		}
		else if ((_codi == SUMA or _codi == RESTA) and 
				(t._codi == SUMA or t._codi == RESTA)) { 
			b = false;
		}
		else {
			b = (_codi > t._codi);
		}
	}

	return b;
}

/*  COST: O(1).	*/
bool token::operator<(const token & t) const throw(error)
{
	bool b = false;
	if ((_codi != EXPONENCIACIO and _codi != CANVI_DE_SIGNE and _codi != SIGNE_POSITIU and 
		 _codi != MULTIPLICACIO and _codi != DIVISIO and _codi != SUMA and _codi != RESTA) or (
		 t._codi != EXPONENCIACIO and t._codi != CANVI_DE_SIGNE and t._codi != SIGNE_POSITIU and 
		 t._codi != MULTIPLICACIO and t._codi != DIVISIO and t._codi != SUMA and t._codi != RESTA))
	{
		throw error(PrecedenciaEntreNoOperadors);
	}
	else {
		if (t._codi == _codi)
			b = false;
		// MATEIX NIVELL
		else if ((_codi == CANVI_DE_SIGNE or _codi == SIGNE_POSITIU) and 
				(t._codi == CANVI_DE_SIGNE or t._codi == SIGNE_POSITIU)) { 
			b = false;
		}
		else if ((_codi == MULTIPLICACIO or _codi == DIVISIO) and 
				(t._codi == MULTIPLICACIO or t._codi == DIVISIO)) { 
			b = false;
		}
		else if ((_codi == SUMA or _codi == RESTA) and 
				(t._codi == SUMA or t._codi == RESTA)) { 
			b = false;
		}
		else {
			b = !(*this > t);
		}
	}
	return b;
}
