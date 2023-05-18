#include "variables.hpp"
#include "math_io.hpp"

using namespace std;

/*	Construeix un conjunt de variables buit.
	COST: O(_M).	*/
variables::variables() throw(error)
{
	_M = 60;
	_taula = new node_hash *[_M];
	for (nat i = 0; i < _M; ++i) {
		_taula [i] = NULL;
	}
	_quants = 0;
}

/* Constructora per còpia.	
	COST: O(1).	*/ 
variables::variables(const variables & v) throw(error)
{
	(*this) = v;
}

/* 	Assignació.	
	COST: O(1)	*/ 
variables & variables::operator=(const variables & v) throw(error)
{
	_taula = v._taula;
	_M = v._M;
	_quants = v._quants;
	return (*this);
}

/* Destructora.	
	COST: O(0).	*/ 
variables::~variables() throw(error)
{
}

/*	Afegeix al conjunt de variables la variable de nom v juntament amb el seu
	valor-expressió e. Si la variable v ja existia llavors li assigna el nou
	valor-expressió. 
	COST: O(1).	si factor carrega = _quants/_M	és un valor raonablement petit.	*/
void variables::assign(const string & v, const expressio & e) throw(error)
{
	int i = util::hash(v) % _M;
	node_hash *p = _taula[i];
	bool trobat = false;
	while (p != NULL and not trobat) {
		if (p->_var == v) {
			trobat = true;
			p->_expr = e;
		}
		else {
			p = p->_seg;
		}
	}
	if (trobat) {	
		// Només canviem el valor assignat
		p->_var = v;
	}
	else {
		// Cal crear un nou node i l’afegim al principi
		node_hash *ant(_taula[i]);
		_taula[i] = new node_hash;
		_taula[i]->_var = v;
		_taula[i]->_expr = e;
		_taula[i]->_seg = ant;
		++_quants;
	}
}

/* 	Elimina del conjunt de variables el parell amb la variable de nom v. Si
	la variable no existeix llavors no fa res. 
	COST: O(1).	si factor carrega = _quants/_M	és un valor raonablement petit.	*/
void variables::unassign(const string & v) throw()
{
	nat i = util::hash(v) % _M;
	node_hash *p = _taula[i], *ant = NULL;
	bool trobat = false;
	while (p != NULL and not trobat) {
		if (p->_var == v) {
			trobat = true;
		}
		else {
			ant = p;
			p = p->_seg;
		}
	}
	if (trobat) {
		if (ant == NULL) {
			_taula[i] = p->_seg; // Era el primer
		}
		else {
			ant->_seg = p->_seg;
		}
		delete (p);
		--_quants;
	}
}

/*	Consulta el valor-expressió de la variable v. Si la variable no està en
 	el conjunt de variables retorna l'expressió buida. 
	COST: O(_quants).	*/
expressio variables::valor(const string & lv) const throw(error)
{
	bool hi_es = false;
	int i = util::hash(lv) % _M;
	node_hash *p = _taula[i];
	expressio e;
	while (p != NULL and not hi_es) {
		if (p->_var == lv) {
			hi_es = true;
			e = p->_expr;
		}
		else {
			p = p->_seg;
		}
	}
	return e;
}

/* 	Retorna en l totes les claus del conjunt de variables, en un ordre
	qualsevol. Si no hi ha cap clau retorna la llista buida.	
	COST: O(_quants).	*/
void variables::dump(list<string> & l) const throw(error)
{
	node_hash *p(NULL);
	for (nat i = 0; i < _M; i++) {
		p = _taula[i];
		while (p != NULL) {
			l.push_back(p->_var);
			p = p->_seg;
		}
	}
}
