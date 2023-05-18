#include "expressio.hpp"
#include <stack>
#include <cmath>
#include "math_io.hpp"
#include <iomanip>
#include <cstdio>

using namespace std;

//-----------Metodes privats o adicionals:

/*	Funcio auxiliar que indica si un token es operador segons el seu codi.	
	Codi Operand: SUMA, RESTA, MULTIPLICACIO, DIVISIO, EXPONENCIACIO,
				  CANVI_DE_SIGNE, SIGNE_POSITIU, OBRIR_PAR, COMA,  
		   		  SQRT, LOG, EXP, EVALF	*/
bool check_op(token::codi cod) 
//PRE: -
//POST: Retorna True sii cod es el codi d'un token operador:
//		(+, -, *, /, ^, -, +, (, sqrt, log, exp i evalf).
//		Retorna False en cas contari.
{
	return cod == token::SUMA or cod == token::RESTA or
		   cod == token::MULTIPLICACIO or cod == token::DIVISIO or 
		   cod == token::EXPONENCIACIO or cod == token::CANVI_DE_SIGNE or 
		   cod == token::SIGNE_POSITIU or cod == token::OBRIR_PAR or cod == token::COMA or 
		   cod == token::SQRT or cod == token::LOG or 
		   cod == token::EXP or cod == token::EVALF;
};

/*	Funcio auxiliar que indica si un token es operand segons el seu codi.	
	Codi Operand: CT_ENTERA, CT_RACIONAL, CT_REAL, CT_E, VARIABLE, VAR_PERCENTATGE*/
bool check_operand(token::codi cod)
//PRE: -
//POST: Retorna True sii cod es el codi d'un token operand:
//		(enter, racional, real, e, variable i percentatge).
//		Retorna False en cas contari.
{
	return cod == token::CT_ENTERA or
		   cod == token::CT_RACIONAL or
		   cod == token::CT_REAL or
		   cod == token::CT_E or
		   cod == token::VARIABLE or
		   cod == token::VAR_PERCENTATGE;
};

/*	Metode auxiliar utilitzat a vars(): Es fa el recorregut en preordre a partir de 
  	node p. Si p es un token del tipus VARIABLE es guarda el seu identificador a la llista l.*/
void expressio::rec_preordre(node *p, list<string> &l)
//PRE: -
//POST: Si p es un token del tipus VARIABLE es guarda el seu identificador a la llista l.
//		Fent un recorregut en preordre.
{
	if (p != NULL)
	{
		bool trobat = false;
		if (p->info.tipus() == token::VARIABLE) {
			if (l.empty())
				l.push_back(p->info.identificador_variable());

			list<string>::iterator it;
			for (it = l.begin(); not trobat and it != l.end(); ++it) {
				trobat = (*it == p->info.identificador_variable());
			}
			if (not trobat)
				l.push_back(p->info.identificador_variable());
		}
		rec_preordre(p->f_esq, l);
		rec_preordre(p->f_dret, l);
	}
};

/* 	Metode auxiliar utilitzat a operator==, simplify_subtree(node * p), 
	simplify_one_step(bool & simplificat, node *p) i simplify(node *p):
	Compara dues expressions començant per les seves respectives arrels a i b.
*/
bool expressio::arbresIguals(node *a, node *b)
//PRE: -
//POST: Retorna True sii les dues expressions que comencen per a i b son iguals.
//		Altrament, retorna False.
{
	if (a == NULL and b == NULL)
		return true;

	if (a != NULL and b != NULL) {
		return (a->info == b->info and
				arbresIguals(a->f_esq, b->f_esq) and
				arbresIguals(a->f_dret, b->f_dret));
	}

	return false;
};

/*	Copia tots els nodes de l'expressio que comenca per node m
	i guarda el seu valor a l'expressio implicita.	*/
expressio::node *expressio::copia_nodes(node *m)
//PRE: -
//POST: Si m és NULL, el resultat és NULL; 
//		sinó, el resultat apunta al primer node d'un arbre binari
//    	de nodes que són còpia de l'arbre apuntat per m.
{
	node *n;
	if (m == NULL)
		n = NULL;
	else {
		n = new node;
		try {
			n->info = m->info;
			n->f_esq = copia_nodes(m->f_esq);
			n->f_dret = copia_nodes(m->f_dret);
		}
		catch (...) {
			delete n;
			throw;
		}
	}
	return n;
};

/*	Esborra tots els nodes de l'expressio que comenca per node m.	*/
void expressio::esborra_nodes(node *m)
//PRE: - 
//POST: No fa res si m és NULL, sinó allibera
//		espai dels nodes del BST expressio apuntat per m */
{
	if (m != NULL) {
		esborra_nodes(m->f_esq);
		esborra_nodes(m->f_dret);
		delete m;
	}
};

/*	Constructora auxiliar de expressio a partir de dues expressions i un token.	*/
expressio::expressio(expressio &ee, const token &t, expressio &ed)
// PRE: -
// POST: El resultat és una expressio amb t com arrel, ee com a fill
//		 esquerre i ed com a fill dret. No fa còpia de les expressions ee i ed.
{
	_arrel = new node;
	try {
		_arrel->info = t;
	}
	catch (...) {
		delete _arrel;
		throw;
	}
	_arrel->f_esq = ee._arrel;
	ee._arrel = NULL;
	_arrel->f_dret = ed._arrel;
	ed._arrel = NULL;
};

/*	Indica si el token t es un operador unari.	*/
bool expressio::es_operador_unari(token t)
//PRE: -
//POST: Retorna True sii t es un token operador unari:
//		(-, +, sqrt, log, exp i evalf).
//		Retorna False en cas contari.
{
	bool b = (t.tipus() == token::CANVI_DE_SIGNE or
			  t.tipus() == token::SIGNE_POSITIU or
			  t.tipus() == token::SQRT or
			  t.tipus() == token::LOG or
			  t.tipus() == token::EXP or
			  t.tipus() == token::EVALF);
	return b;
};

/*	Indica si el token t es un operador binari.	*/
bool expressio::es_operador_binari(token t)
//PRE: -
//POST: Retorna True sii t es un token operador binari:
//		(+, -, *, /, ^).
//		Retorna False en cas contari.
{
	bool b = (t.tipus() == token::SUMA or
			  t.tipus() == token::RESTA or
			  t.tipus() == token::MULTIPLICACIO or
			  t.tipus() == token::DIVISIO or
			  t.tipus() == token::EXPONENCIACIO);

	return b;
};

/*	Metode auxiliar utilitzat a imprimir_expressio().
	Imprimeix l'expressio fent un recorregut en
	inordre especular. Cada linia representa un fill.	*/
void expressio::impr(node *n, int nivell)
//PRE: -
//POST: Imprimeix l'expressio començant per n. Fent un recorregut en
//		inordre especular. Cada linia representa un fill.
{
	if (n != NULL) {
		impr(n->f_dret, nivell + 1);
		cout << setw(nivell * 10) << token_tostring(n->info) << endl;
		impr(n->f_esq, nivell + 1);
	}
	cout << endl;
};

/*	Imprimeix l'expressio e fent ús de la funció impr().	*/
void expressio::imprimir_expressio(const expressio &e)
//PRE: -
//POST: Imprimeix l'expressio e fent ús de la funció impr().
{
	int nivell = 1;
	impr(e._arrel, nivell);
};

/*	Transforma el token it en un string.	*/
string expressio::token_tostring(const token &it)
//PRE: it es un token.
//POST: Retorna un string que representa el token it.
{
	char buf[20];
	string s_exp, s_tok, s;

	switch (it.tipus()) {
		case token::NULLTOK:
			break;
		case token::CT_ENTERA:
			sprintf(buf, "%i", it.valor_enter());
			s_tok = string(buf);
			break;
		case token::CT_RACIONAL:
			sprintf(buf, "%i", it.valor_racional().num());
			s = string(buf);
			sprintf(buf, "%i", it.valor_racional().denom());
			s_tok = s + "/" + string(buf);
			break;
		case token::CT_REAL:
			sprintf(buf, "%f", it.valor_real());
			s_tok = string(buf);
			break;
		case token::CT_E:
			s_tok = "e";
			break;
		case token::VARIABLE:
			s_tok = it.identificador_variable();
			break;
		case token::VAR_PERCENTATGE:
			s_tok = "%";
			break;
		case token::SUMA:
			s_tok = "+";
			break;
		case token::RESTA:
			s_tok = "-";
			break;
		case token::MULTIPLICACIO:
			s_tok = "*";
			break;
		case token::DIVISIO:
			s_tok = "/";
			break;
		case token::EXPONENCIACIO:
			s_tok = "^";
			break;
		case token::CANVI_DE_SIGNE:
			s_tok = "-";
			break;
		case token::SIGNE_POSITIU:
			s_tok = "+";
			break;
		case token::SQRT:
			s_tok = "sqrt";
			break;
		case token::EXP:
			s_tok = "exp";
			break;
		case token::LOG:
			s_tok = "log";
			break;
		case token::EVALF:
			s_tok = "evalf";
			break;
		case token::OBRIR_PAR:
			s_tok = "(";
			break;
		case token::TANCAR_PAR:
			s_tok = ")";
			break;
		case token::COMA:
			s_tok = ",";
			break;
		case token::ASSIGNACIO:
			s_tok = ":=";
			break;
		case token::DESASSIGNACIO:
			s_tok = "unassign ";
			break;
		case token::BYEBYE:
			s_tok = "byebye";
	}
	s_exp = s_exp + s_tok;

	return s_exp;
};

/*	Comproba si l'iterador it i el seu següent element de la llista l són sintàcticament correctes.	*/
bool expressio::consecutius_valids(list<token>::const_iterator it, const list<token> &l)
//PRE: -
//POST: Retorna True si l'iterador it i el seu següent element de la llista l són sintàcticament correctes.
//		Altrament, retorna false.
{
	bool es_valid = true;
	token ant, seg;
	ant = *(it);
	if (l.size() > 1) {
		list<token>::const_iterator it_aux = it;
		if (++it_aux != l.end()) {
			seg = *(++it);
			if ((es_operador_unari(ant) and ant.tipus() != token::CANVI_DE_SIGNE and ant.tipus() != token::SIGNE_POSITIU) and seg.tipus() != token::OBRIR_PAR) {
				es_valid = false;
			}
			else if (check_operand(ant.tipus()) and check_operand(seg.tipus())){
				es_valid = false;
			}
			else if (es_operador_binari(ant) and es_operador_binari(seg)){
				es_valid = false;
			}
			else if (es_operador_unari(ant) and es_operador_binari(seg)){
				es_valid = false;
			}
			else
				es_valid = true;
		}
	}
	return es_valid;
};

/*	Metode auxiliar utilitzat a apply_substitution().
	Comprova si la variable v està al node p, i si es així substitueix aquest node per l'arrel de l'expressio e.	*/
expressio::node *expressio::cerca(node *p, const string &v, const expressio &e)
//PRE: -
//POST: Comprova si la variable v està al node p, i si es així substitueix aquest node per l'arrel de l'expressio e.
{
	if (p != NULL) {
		if (p->info.tipus() == token::VARIABLE) {
			if (p->info.identificador_variable() == v) {
				p = e._arrel;
			}
		}
		p->f_esq = cerca(p->f_esq, v, e);
		p->f_dret = cerca(p->f_dret, v, e);
	}
	return p;
};

/*	Metode auxiliar utilitzat a list_of_tokens().
	Es fa un recorregut en inordre de la expressio per imprimir l'expressio amb els
	parentesis necessaris.	*/
void expressio::list_of_tokens_aux(node *p, list<token> &l) const
//PRE: p apunta a un node d'una expressio.
//POST:	Fica a l un token de l'expressio implícita en inordre amb els parentesis necessaris. 
{
	if (this->_arrel->info.tipus() == token::NULLTOK) {
		l.push_back(this->_arrel->info);
	}
	else if (p->info.tipus() != token::NULLTOK) {
		if (check_op(p->info.tipus())) {
			if(es_operador_binari(p->info)) {
				if (check_op(p->f_esq->info.tipus())) {
					if (p->info > p->f_esq->info) {
						l.push_back(token(token::OBRIR_PAR));
						list_of_tokens_aux(p->f_esq, l);
						l.push_back(token(token::TANCAR_PAR));
					}
					else {
						if (p->info.tipus() == token::EXPONENCIACIO and 
						   p->f_esq->info.tipus() == token::EXPONENCIACIO) {
							l.push_back(token(token::OBRIR_PAR));
							list_of_tokens_aux(p->f_esq, l);
							l.push_back(token(token::TANCAR_PAR));
						}
						else 
							list_of_tokens_aux(p->f_esq, l);
					}
				}
				else if (check_operand(p->f_esq->info.tipus())) {
					if (p->info.tipus() == token::EXPONENCIACIO and p->f_esq->info.tipus() == token::CT_RACIONAL) {
						if(p->f_dret->info.tipus() == token::EXPONENCIACIO or p->f_dret->info.tipus() == token::CT_RACIONAL)
							list_of_tokens_aux(p->f_esq, l);
						else {
							l.push_back(token(token::OBRIR_PAR));
							list_of_tokens_aux(p->f_esq, l);
							l.push_back(token(token::TANCAR_PAR));
						}
					}
					else {
						list_of_tokens_aux(p->f_esq, l);
					}
				}
				l.push_back(p->info);
				if (check_op(p->f_dret->info.tipus())) {
					if ((es_operador_binari(p->info) or p->info.tipus() == token::CANVI_DE_SIGNE or p->info.tipus() == token::SIGNE_POSITIU) 
					and (es_operador_binari(p->f_dret->info) or	p->f_dret->info.tipus() == token::CANVI_DE_SIGNE or p->f_dret->info.tipus() == token::SIGNE_POSITIU)) {
						if (p->info > p->f_dret->info) {
							if (es_operador_binari(p->f_dret->info)) {
								l.push_back(token(token::OBRIR_PAR));
								list_of_tokens_aux(p->f_dret, l);
								l.push_back(token(token::TANCAR_PAR));
							}
							else if (es_operador_unari(p->f_dret->info)) {
								list_of_tokens_aux(p->f_dret, l);
							}
						}
						else {
							if (p->info.tipus() == token::RESTA and p->f_dret->info.tipus() == token::SUMA) {
								l.push_back(token(token::OBRIR_PAR));
								list_of_tokens_aux(p->f_dret, l);
								l.push_back(token(token::TANCAR_PAR));
							}
							else if (p->info.tipus() == token::RESTA and p->f_dret->info.tipus() == token::RESTA) {
								l.push_back(token(token::OBRIR_PAR));
								list_of_tokens_aux(p->f_dret, l);
								l.push_back(token(token::TANCAR_PAR));
							}
							else if (p->info.tipus() == token::DIVISIO and p->f_dret->info.tipus() == token::MULTIPLICACIO) {
								l.push_back(token(token::OBRIR_PAR));
								list_of_tokens_aux(p->f_dret, l);
								l.push_back(token(token::TANCAR_PAR));
							}
							else if (p->info.tipus() == token::DIVISIO and p->f_dret->info.tipus() == token::DIVISIO) {
								l.push_back(token(token::OBRIR_PAR));
								list_of_tokens_aux(p->f_dret, l);
								l.push_back(token(token::TANCAR_PAR));
							}
							else 
								list_of_tokens_aux(p->f_dret, l);
						}
					}
					else {
						if (es_operador_binari(p->f_dret->info)) {
							l.push_back(token(token::OBRIR_PAR));
							list_of_tokens_aux(p->f_dret, l);
							l.push_back(token(token::TANCAR_PAR));
						}
						else if (es_operador_unari(p->f_dret->info)) {
							list_of_tokens_aux(p->f_dret, l);
						}
					}
				}
				else if (check_operand(p->f_dret->info.tipus())) {
					if(p->f_dret->info.tipus() == token::CT_RACIONAL){
						l.push_back(token(token::OBRIR_PAR));
						list_of_tokens_aux(p->f_dret, l);
						l.push_back(token(token::TANCAR_PAR));
					}
					else
						list_of_tokens_aux(p->f_dret, l);
				}
			}
			else if (es_operador_unari(p->info)) {
				l.push_back(p->info);
				if (check_op(p->f_dret->info.tipus())) {
					if(p->info.tipus() == token::CANVI_DE_SIGNE and p->f_dret->info.tipus() == token::EXPONENCIACIO){
						list_of_tokens_aux(p->f_dret, l);
					}
					else{
						l.push_back(token(token::OBRIR_PAR));
						list_of_tokens_aux(p->f_dret, l);
						l.push_back(token(token::TANCAR_PAR));
					}
				}
				else if (check_operand(p->f_dret->info.tipus())) {
					if(p->info.tipus() == token::SQRT or p->info.tipus() == token::LOG){
						l.push_back(token(token::OBRIR_PAR));
						list_of_tokens_aux(p->f_dret, l);
						l.push_back(token(token::TANCAR_PAR));	
					}
					else{
						list_of_tokens_aux(p->f_dret, l);
					}
				}
			}
		}
		else {
			l.push_back(p->info);
		}
	}
};

/*	Metode auxilar que conte totes les regles matematiques per simplificar expressions.	*/
expressio::node *expressio::simplify_subtree(node *p)
//PRE: p apunta a un node d'una expressio a la que es vol realitzar la simplificacio.
//POST: retorna l'arrel de l'expressio resultant d'aplicar les simplificacions desitjades.
{
	if (p != NULL)
	{
		node *nou(p);
		expressio buida = expressio();

		/* 1. Per qualsevol operació binària op ∈ {+, −, ∗, /} i qualssevol constants c 1 i c 2 del
			mateix tipus,
			
					  op		→ c1 op c2
					c1 	 c2

			En el cas que les constants c 1 i c 2 siguin enters i l’operació op sigui la divisió llavors
			c 1 op c 2 és una constant racional si el quocient no és exacte. Si una de les constants
			és entera i l’altra racional la regla també pot ser aplicada convertint prèviament la
			constant entera al seu equivalent racional. Es produeix un error si op = / i c 2 = 0
			(com enter, racional o real). 
		*/
		if (es_operador_binari(p->info)) {
			if (p->info.tipus() == token::SUMA) {
				if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_dret->info.tipus() == token::CT_ENTERA) {
					nou = expressio(token(p->f_esq->info.valor_enter() + p->f_dret->info.valor_enter()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_dret->info.tipus() == token::CT_REAL) {
					nou = expressio(token(p->f_esq->info.valor_real() + p->f_dret->info.valor_real()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_dret->info.tipus() == token::CT_RACIONAL) {
					nou = expressio(token(p->f_esq->info.valor_racional() + p->f_dret->info.valor_racional()))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->info.tipus() == token::RESTA) {
				if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_dret->info.tipus() == token::CT_ENTERA) {
					nou = expressio(token(p->f_esq->info.valor_enter() - p->f_dret->info.valor_enter()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_dret->info.tipus() == token::CT_REAL) {
					nou = expressio(token(p->f_esq->info.valor_real() - p->f_dret->info.valor_real()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_dret->info.tipus() == token::CT_RACIONAL) {
					nou = expressio(token(p->f_esq->info.valor_racional() - p->f_dret->info.valor_racional()))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->info.tipus() == token::MULTIPLICACIO) {
				if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_dret->info.tipus() == token::CT_ENTERA) {
					nou = expressio(token(p->f_esq->info.valor_enter() * p->f_dret->info.valor_enter()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_dret->info.tipus() == token::CT_REAL) {
					nou = expressio(token(p->f_esq->info.valor_real() * p->f_dret->info.valor_real()))._arrel;
					esborra_nodes(p);
				}
				else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_dret->info.tipus() == token::CT_RACIONAL){
					nou = expressio(token(p->f_esq->info.valor_racional() * p->f_dret->info.valor_racional()))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->info.tipus() == token::DIVISIO) {
				if (p->f_dret->info.tipus() == token::CT_ENTERA and 
					p->f_esq->info.tipus() == token::CT_ENTERA and 
				   	p->f_dret->info.valor_enter() != 0 and 
				   	p->f_dret->info.valor_enter() != 1 and 
				   	p->f_esq->info.valor_enter() != 0) {
				   		if ((p->f_esq->info.valor_enter() % p->f_dret->info.valor_enter()) == 0) {
							nou = expressio(token(p->f_esq->info.valor_enter() / p->f_dret->info.valor_enter()))._arrel;
							esborra_nodes(p);
						} 
						else {
							token rac(racional(p->f_esq->info.valor_enter(), p->f_dret->info.valor_enter()));
							nou = expressio(rac)._arrel;
							esborra_nodes(p);
						}
					}
				else if (p->f_esq->info.tipus() == token::CT_REAL and 
				 		 p->f_dret->info.tipus() == token::CT_REAL and 
						 p->f_dret->info.valor_real() != 0 and 
						 p->f_dret->info.valor_real() != 1 and 
						 p->f_esq->info.valor_real() != 0) {
							nou = expressio(token(p->f_esq->info.valor_real() / p->f_dret->info.valor_real()))._arrel;
							esborra_nodes(p);
						 }
				else if (p->f_esq->info.tipus() == token::CT_RACIONAL and 
						 p->f_dret->info.tipus() == token::CT_RACIONAL and 
						 p->f_dret->info.valor_racional().num() != 0 and 
						 p->f_esq->info.valor_racional().num() != 0 and
						 (p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) != 1) {
							nou = expressio(token(p->f_esq->info.valor_racional() / p->f_dret->info.valor_racional()))._arrel;
							esborra_nodes(p);
						 }
			}
		}
		// 2. Regles SUMA:
		if (p->info.tipus() == token::SUMA) {
			// 2. a) 0 + E → E
			if (p->f_esq->info.tipus() == token::CT_ENTERA) {
				if (p->f_esq->info.valor_enter() == 0) {
					nou = p->f_dret;
					esborra_nodes(p->f_esq);
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_RACIONAL) {
				if (p->f_esq->info.valor_racional().num() == 0) {
					nou = p->f_dret;
					esborra_nodes(p->f_esq);
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL) {
				if (p->f_esq->info.valor_real() == 0) {
					nou = p->f_dret;
					esborra_nodes(p->f_esq);
				}
			}
			// 2. b) E + 0 → E
			if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				if (p->f_dret->info.valor_enter() == 0) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				if (p->f_dret->info.valor_racional().num() == 0) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL) {
				if (p->f_dret->info.valor_real() == 0) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			// 2. c) E + E → 2 ∗ E
			if (arbresIguals(p->f_esq, p->f_dret)) {
				expressio dos(token(2));
				nou = expressio(dos, token(token::MULTIPLICACIO), buida)._arrel;
				nou->f_dret = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			//2. d) E + (−E') → E − E'
			else if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				nou = expressio(buida, token(token::RESTA), buida)._arrel;
				nou->f_esq = p->f_esq;
				nou->f_dret = p->f_dret->f_dret;
			}
			// 2. e) (−E') + E → E − E'
			else if (p->f_esq->info.tipus() == token::CANVI_DE_SIGNE) {
				nou = expressio(buida, token(token::RESTA), buida)._arrel;
				nou->f_esq = p->f_esq->f_dret;
				nou->f_dret = p->f_dret;
			}
			// 2. f) 
			else if (p->f_esq->info.tipus() == token::MULTIPLICACIO and p->f_dret->info.tipus() == token::MULTIPLICACIO) {
				// 2. f) E1 ∗ E + E2 ∗ E → (E1 + E2) ∗ E
				if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_dret)) {
					expressio ee(buida, token(token::SUMA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_dret);
				}
				// 2. g) E1 ∗ E + E ∗ E2 → (E1 + E2) ∗ E
				else if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_esq)) {
					expressio ee(buida, token(token::SUMA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(ee, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_dret = p->f_esq->f_dret;
					esborra_nodes(p->f_dret->f_esq);
				}
				// 2. h) E ∗ E1 + E2 ∗ E → (E1 + E2) ∗ E
				else if (arbresIguals(p->f_dret->f_dret, p->f_esq->f_esq)) {
					expressio ee(buida, token(token::SUMA), buida);
					ee._arrel->f_esq = p->f_esq->f_dret;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_esq);
				}
				// 2. i) E ∗ E1 + E ∗ E2 → (E1 + E2) ∗ E
				else if (arbresIguals(p->f_esq->f_esq, p->f_dret->f_esq)) { 
					expressio ee(buida, token(token::SUMA), buida);
					ee._arrel->f_esq = p->f_esq->f_dret;
					ee._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_esq;
					esborra_nodes(p->f_esq->f_esq);
				}
			}
			// 2. j) E1 / E + E2 / E → (E1 + E2) / E
			else if (p->f_esq->info.tipus() == token::DIVISIO and p->f_dret->info.tipus() == token::DIVISIO) {
				if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_dret)) {
					expressio ee(buida, token(token::SUMA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_esq->f_dret;
					esborra_nodes(p->f_dret->f_dret);
				}
			}
		}
		// 3. Regles RESTA:
		else if (p->info.tipus() == token::RESTA) {
			// 3. a) 0 − E → −E (canvi de signe)
			if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_esq->info.valor_enter() == 0) {
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), buida)._arrel;
				nou->f_dret = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_esq->info.valor_racional().num() == 0) {
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), buida)._arrel;
				nou->f_dret = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_esq->info.valor_real() == 0) {
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), buida)._arrel;
				nou->f_dret = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			// 3. b) E − 0 → E
			else if (p->f_dret->info.tipus() == token::CT_ENTERA and p->f_dret->info.valor_enter() == 0) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL and p->f_dret->info.valor_racional().num() == 0) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL and p->f_dret->info.valor_real() == 0) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			// 3. c) E − E → 0
			else if (arbresIguals(p->f_esq, p->f_dret)) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			// 3. d) E − (−E′) → E + E′
			else if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				nou = expressio(buida, token(token::SUMA), buida)._arrel;
				nou->f_esq = p->f_esq;
				nou->f_dret = p->f_dret->f_dret;
			}
			else if (p->f_esq->info.tipus() == token::MULTIPLICACIO and p->f_dret->info.tipus() == token::MULTIPLICACIO) {
				// 3. e) E1 ∗ E − E2 ∗ E → (E1 − E2) ∗ E
				if (arbresIguals(p->f_dret->f_dret, p->f_esq->f_dret)) {
					expressio ee(buida, token(token::RESTA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_dret);
				}
				// 3. f) E1 ∗ E − E ∗ E2 → (E1 − E2 ) ∗ E
				else if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_esq)) {
					expressio ee(buida, token(token::RESTA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(ee, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_dret = p->f_esq->f_dret;
					esborra_nodes(p->f_dret->f_esq);
				}
				// 3. g) E ∗ E1 − E2 ∗ E → (E1 − E2) ∗ E
				else if (arbresIguals(p->f_dret->f_dret, p->f_esq->f_esq)) {
					expressio ee(buida, token(token::RESTA), buida);
					ee._arrel->f_esq = p->f_esq->f_dret;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_esq);
				}
				// 3. h) E ∗ E1 − E ∗ E2 → (E1 − E2) ∗ E
				else if (arbresIguals(p->f_esq->f_esq, p->f_dret->f_esq)) {
					expressio ee(buida, token(token::RESTA), buida);
					ee._arrel->f_esq = p->f_esq->f_dret;
					ee._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(ee, token(token::MULTIPLICACIO), buida)._arrel;
					nou->f_dret = p->f_dret->f_esq;
					esborra_nodes(p->f_esq->f_esq);
				}
			}
			// 3. i) E1 / E − E2 / E → (E1 − E2) / E
			else if (p->f_esq->info.tipus() == token::DIVISIO and p->f_dret->info.tipus() == token::DIVISIO) {
				if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_dret)) {
					expressio ee(buida, token(token::RESTA), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(ee, token(token::DIVISIO), buida)._arrel;
					nou->f_dret = p->f_esq->f_dret;
					esborra_nodes(p->f_dret->f_dret);
				}
			}
		}
		// // 4. REGLES CAVI DE SIGNE I SIGNE POSITIU
		else if (p->info.tipus() == token::CANVI_DE_SIGNE) {
			// 4. a) −(−E) → E
			if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				nou = p->f_dret->f_dret;
			}
			// 4. c) Per qualsevol constant c (entera, racional o real)
			else if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				token aux(p->f_dret->info.valor_enter() * (-1));
				nou = expressio(aux)._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				racional r(p->f_dret->info.valor_racional().num() * (-1), p->f_dret->info.valor_racional().denom());
				token aux(r);
				nou = expressio(aux)._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL) {
				token aux(p->f_dret->info.valor_real() * (-1));
				nou = expressio(aux)._arrel;
				esborra_nodes(p);
			}
		}
		// 4. b) +E → E
		else if (p->info.tipus() == token::SIGNE_POSITIU) {
			nou = p->f_dret;
		}
		// 5. Regles MULTIPLICACIO
		else if (p->info.tipus() == token::MULTIPLICACIO) {
			// 5. a) 1 ∗ E → E
			if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_esq->info.valor_enter() == 1) {
				nou = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			else if (p->f_esq->info.tipus() == token::CT_RACIONAL) {
				if ((p->f_esq->info.valor_racional().num() / p->f_esq->info.valor_racional().denom()) == 1) {
					nou = p->f_dret;
					esborra_nodes(p->f_esq);
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_esq->info.valor_real() == 1) {
				nou = p->f_dret;
				esborra_nodes(p->f_esq);
			}
			// 5. b) E ∗ 1 → E
			else if (p->f_dret->info.tipus() == token::CT_ENTERA and p->f_dret->info.valor_enter() == 1) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				if ((p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) == 1) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL and p->f_dret->info.valor_real() == 1) {
				nou = p->f_esq;
			}
			// 5. c) 0 ∗ E → 0
			else if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_esq->info.valor_enter() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_esq->info.valor_racional().num() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_esq->info.valor_real() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			// 5. d) E ∗ 0 → 0
			else if (p->f_dret->info.tipus() == token::CT_ENTERA and p->f_dret->info.valor_enter() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL and p->f_dret->info.valor_racional().num() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL and p->f_dret->info.valor_real() == 0) {
				nou = expressio(token(0))._arrel;
				esborra_nodes(p);
			}
			// 5. e) E ∗ E → Eˆ2
			else if (arbresIguals(p->f_dret, p->f_esq)) {
				nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
				nou->f_esq = p->f_dret;
				nou->f_dret = expressio(token(2))._arrel;
				esborra_nodes(p->f_esq);
			}
			// 5. f) E ∗ (−E') → −(E ∗ E')
			else if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				expressio ed(buida, token(token::MULTIPLICACIO), buida);
				ed._arrel->f_esq = p->f_esq;
				ed._arrel->f_dret = p->f_dret->f_dret;
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), buida)._arrel;
				nou->f_dret = ed._arrel;
			}
			// 5. g) (−E) ∗ E′ → −(E ∗ E′)
			else if (p->f_esq->info.tipus() == token::CANVI_DE_SIGNE) {
				expressio ed(buida, token(token::MULTIPLICACIO), buida);
				ed._arrel->f_esq = p->f_esq->f_dret;
				ed._arrel->f_dret = p->f_dret;
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), buida)._arrel;
				nou->f_dret = ed._arrel;
			}
			// 5. h) E ∗ (1/E′) → E/E′
			else if (p->f_dret->info.tipus() == token::DIVISIO) {
				if (p->f_dret->f_esq->info.tipus() == token::CT_ENTERA) {
					if (p->f_dret->f_esq->info.valor_enter() == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
					}
				}
				else if (p->f_dret->f_esq->info.tipus() == token::CT_REAL) {
					if (p->f_dret->f_esq->info.valor_real() == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
					}
				}
				else if (p->f_dret->f_esq->info.tipus() == token::CT_RACIONAL) {
					if ((p->f_dret->f_esq->info.valor_racional().num() / p->f_dret->f_esq->info.valor_racional().denom()) == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
					}
				}
			}
			// 5. i) (1/E′) ∗ E → E/E′
			else if (p->f_esq->info.tipus() == token::DIVISIO) {
				if (p->f_esq->f_esq->info.tipus() == token::CT_ENTERA) {
					if (p->f_esq->f_esq->info.valor_enter() == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_dret;
						nou->f_dret = p->f_esq->f_dret;
						esborra_nodes(p->f_esq->f_esq);
					}
				}
				else if (p->f_esq->f_esq->info.tipus() == token::CT_REAL) {
					if (p->f_esq->f_esq->info.valor_real() == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_dret;
						nou->f_dret = p->f_esq->f_dret;
						esborra_nodes(p->f_esq->f_esq);
					}
				}
				else if (p->f_esq->f_esq->info.tipus() == token::CT_RACIONAL) {
					if ((p->f_esq->f_esq->info.valor_racional().num() / p->f_esq->f_esq->info.valor_racional().denom()) == 1) {
						nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
						nou->f_esq = p->f_dret;
						nou->f_dret = p->f_esq->f_dret;
						esborra_nodes(p->f_esq->f_esq);
					}
				}
			}
			else if (p->f_esq->info.tipus() == token::EXPONENCIACIO and
					 p->f_dret->info.tipus() == token::EXPONENCIACIO) {
				// 5. j) E1ˆE ∗ E2ˆE → (E1 ∗ E2)ˆE
				if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_dret)) {
					expressio ee(buida, token(token::MULTIPLICACIO), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_dret);
				}
				// 5. k) EˆE1 ∗ EˆE2 → Eˆ(E1 + E2)
				else if (arbresIguals(p->f_esq->f_esq, p->f_dret->f_esq)) {
					expressio ed(buida, token(token::SUMA), buida);
					ed._arrel->f_esq = p->f_esq->f_dret;
					ed._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
					nou->f_esq = p->f_dret->f_esq;
					nou->f_dret = ed._arrel;
					esborra_nodes(p->f_esq->f_esq);
				}
			}
			// 5. l) exp(E) ∗ exp(E′) → exp(E + E′)
			else if (p->f_esq->info.tipus() == token::EXP and
					p->f_dret->info.tipus() == token::EXP) {
						expressio ed(buida, token(token::SUMA), buida);
						ed._arrel->f_esq = p->f_esq->f_dret;
						ed._arrel->f_dret = p->f_dret->f_dret;
						nou = expressio(buida, token(token::EXP), buida)._arrel;
						nou->f_esq = expressio()._arrel;
						nou->f_dret = ed._arrel;	
					}
		}
		// 6. Regles de divisió:
		else if (p->info.tipus() == token::DIVISIO) {
			if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				// 6. a) E/0 és un error
				if (p->f_dret->info.valor_enter() == 0) {
					throw error(DivPerZero);
				}
				// 6. c) E/1 → E
				else if (p->f_dret->info.valor_enter() == 1) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				if (p->f_dret->info.valor_racional().num() == 0) {
					nou = expressio(buida, token(token::DIVISIO), buida)._arrel;
					nou->f_esq = p->f_esq;
					nou->f_dret = expressio(token(0))._arrel;
					esborra_nodes(p->f_dret);
				}
				// 6. c) E/1 → E
				if ((p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) == 1) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL) {
				// 6. a) E/0 és un error
				if (p->f_dret->info.valor_real() == 0) {
					throw error(DivPerZero);
				}
				// 6. c) E/1 → E
				else if (p->f_dret->info.valor_real() == 1) {
					nou = p->f_esq;
					esborra_nodes(p->f_dret);
				}
			}
			// 6. b) 0/E → 0
			else if (p->f_esq->info.tipus() == token::CT_ENTERA) {
				if (p->f_esq->info.valor_enter() == 0) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_RACIONAL) {
				if (p->f_esq->info.valor_racional().num() == 0) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL) {
				if (p->f_esq->info.valor_real() == 0) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
			}
			// 6. d) E/E → 1
			else if (arbresIguals(p->f_esq, p->f_dret)) {
				nou = expressio(token(1))._arrel;
				esborra_nodes(p);
			}
			// 6. e) E/(−E') → −(E/E') 
			else if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				expressio ed(buida, token(token::DIVISIO), buida);
				ed._arrel->f_esq = p->f_esq;
				ed._arrel->f_dret = p->f_dret->f_dret;
				nou = expressio(buida, token(token::CANVI_DE_SIGNE), ed)._arrel;
			}
			// 6. f) E/(1/E') → E ∗ E'
			else if (p->f_dret->info.tipus() == token::DIVISIO) {
				if (p->f_dret->f_esq->info.tipus() == token::CT_ENTERA) {
					if (p->f_dret->f_esq->info.valor_enter() == 1) {
						nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
						esborra_nodes(p->f_dret->f_esq);
					}
				}
				else if (p->f_dret->f_esq->info.tipus() == token::CT_RACIONAL) {
					if ((p->f_dret->f_esq->info.valor_racional().num() / p->f_dret->f_esq->info.valor_racional().denom()) == 1) {
						nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
						esborra_nodes(p->f_dret->f_esq);
					}
				}
				else if (p->f_dret->f_esq->info.tipus() == token::CT_REAL) {
					if (p->f_dret->f_esq->info.valor_real() == 1) {
						nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
						nou->f_esq = p->f_esq;
						nou->f_dret = p->f_dret->f_dret;
						esborra_nodes(p->f_dret->f_esq);
					}
				}
			}
			// 6. g) E1ˆE/E2ˆE → (E1/E2)ˆE
			else if (p->f_esq->info.tipus() == token::EXPONENCIACIO and
					 p->f_dret->info.tipus() == token::EXPONENCIACIO)
			{
				// 6. g) E1ˆE/E2ˆE → (E1/E2)ˆE
				if (arbresIguals(p->f_esq->f_dret, p->f_dret->f_dret)) {
					expressio ee(buida, token(token::DIVISIO), buida);
					ee._arrel->f_esq = p->f_esq->f_esq;
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
					nou->f_esq = ee._arrel;
					nou->f_dret = p->f_dret->f_dret;
					esborra_nodes(p->f_esq->f_dret);
				}
				// 6. h) EˆE1/EˆE2 → Eˆ(E1 − E2)
				else if (arbresIguals(p->f_esq->f_esq, p->f_dret->f_esq)) {
					expressio ed(buida, token(token::RESTA), buida);
					ed._arrel->f_esq = p->f_esq->f_dret;
					ed._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
					nou->f_esq = p->f_esq->f_esq;
					nou->f_dret = ed._arrel;
					esborra_nodes(p->f_dret->f_esq);
				}
			}
			// 6. i) exp(E)/exp(E′) → exp(E − E′)
			else if (p->f_esq->info.tipus() == token::EXP and p->f_dret->info.tipus() == token::EXP)
			{
				nou = expressio(buida, token(token::EXP), buida)._arrel;
				nou->f_esq = p->f_esq->f_dret;
				nou->f_dret = p->f_dret->f_dret;
				delete p->f_esq;
				delete p->f_dret;
			}
		}
		// 7. Regles de exponenciació:
		else if (p->info.tipus() == token::EXPONENCIACIO) {
			/* 7. a) Si c2 és una constant de tipus enter, o si c1 i c2 son constants reals llavors
							   ˆ		→ c1ˆc2
							c1 	 c2
							
					Es produeix un error si c1 és una constant negativa (entera, racional o real) i
					c2 no és una constant entera. */
			if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				if (p->f_dret->info.valor_enter() > 0) {
					if (p->f_esq->info.tipus() == token::CT_ENTERA) {
						nou = expressio(token((int)pow(p->f_esq->info.valor_enter(), p->f_dret->info.valor_enter())))._arrel;
						esborra_nodes(p);
					}
					else if (p->f_esq->info.tipus() == token::CT_RACIONAL) {
						racional rac((int)pow(p->f_esq->info.valor_racional().num(), p->f_dret->info.valor_enter()),
									(int)pow(p->f_esq->info.valor_racional().denom(), p->f_dret->info.valor_enter()));
						nou = expressio(token(rac))._arrel;
						esborra_nodes(p);
					}
					else if (p->f_esq->info.tipus() == token::CT_REAL) {
						nou = expressio(token((int)pow(p->f_esq->info.valor_real(), p->f_dret->info.valor_enter())))._arrel;
						esborra_nodes(p);
					}
					else if (p->f_esq->info.tipus() == token::CT_E) {
						nou = expressio(token((int)exp(p->f_dret->info.valor_enter())))._arrel;
						esborra_nodes(p);
					}
				}
				else if (p->f_dret->info.valor_enter() < 0) {
					if (p->f_esq->info.tipus() == token::CT_ENTERA) {
						int res = (int) pow(p->f_esq->info.valor_enter(), abs(p->f_dret->info.valor_enter()));
						racional r(1, res);
						nou = expressio(token(r))._arrel;
					}
					else if (p->f_esq->info.tipus() == token::CT_REAL) {
						double den = pow(p->f_esq->info.valor_real(), abs(p->f_dret->info.valor_enter()));
						double res = 1 / den; 
						nou = expressio(token(res))._arrel;				
					}
					else if (p->f_esq->info.tipus() == token::CT_RACIONAL) {
						int res_num = (int) pow(p->f_esq->info.valor_racional().num(), abs(p->f_dret->info.valor_enter()));
						int res_denom = (int) pow(p->f_esq->info.valor_racional().denom(), abs(p->f_dret->info.valor_enter()));
						racional res(res_denom, res_num);
						if (res_denom % res_num == 0){
							int x = res_denom / res_num;
							nou = expressio(token(x))._arrel;
						}
						else{
							nou = expressio(token(res))._arrel;
						}
					}
				}
			}
			else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_dret->info.tipus() == token::CT_REAL) { 
				nou = expressio(token(pow(p->f_esq->info.valor_real(), p->f_dret->info.valor_real())))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL or 
					 p->f_dret->info.tipus() == token::CT_RACIONAL or
					 p->f_dret->info.tipus() == token::CT_E) {
				if (p->f_esq->info.tipus() == token::CT_ENTERA and p->f_esq->info.valor_enter() < 0) {
			 		throw error(NegatElevNoEnter);
					esborra_nodes(p);
				}
			 	else if (p->f_esq->info.tipus() == token::CT_RACIONAL and p->f_esq->info.valor_racional().num() < 0) {
					throw error(NegatElevNoEnter);
					esborra_nodes(p);
				}
			 	else if (p->f_esq->info.tipus() == token::CT_REAL and p->f_esq->info.valor_real() < 0) {
			 		throw error(NegatElevNoEnter);
					esborra_nodes(p);
				}
			}
			// 7. b) Eˆ0 → 1
			if (p->f_dret->info.tipus() == token::CT_ENTERA and p->f_dret->info.valor_enter() == 0) {
				nou = expressio(token(1))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL and 
					p->f_dret->info.valor_racional().num() == 0) {
				nou = expressio(token(1))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL and p->f_dret->info.valor_real() == 0) {
				nou = expressio(token(1))._arrel;
				esborra_nodes(p);
			}
			// 7. c) Eˆ1 → E
			if (p->f_dret->info.tipus() == token::CT_ENTERA and p->f_dret->info.valor_enter() == 1) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL and p->f_dret->info.valor_real() == 1) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL and 
					(p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) == 1) {
				nou = p->f_esq;
				esborra_nodes(p->f_dret);
			}
			// 7. d) Eˆ − E′ → 1/(EˆE′)
			else if (p->f_dret->info.tipus() == token::CANVI_DE_SIGNE) {
				expressio ee(token(1));
				nou = expressio(ee, token(token::DIVISIO), buida)._arrel;
				expressio ed = expressio(buida, token(token::EXPONENCIACIO), buida);
				ed._arrel->f_esq = p->f_esq;
				ed._arrel->f_dret = p->f_dret->f_dret;
				nou->f_dret = ed._arrel;
			}
			// 7. e) (E1ˆE2)ˆE3 → E1ˆ(E2 ∗ E3)
			else if (p->f_esq->info.tipus() == token::EXPONENCIACIO) {
				expressio ed(buida, token(token::MULTIPLICACIO), buida);
				ed._arrel->f_esq = p->f_esq->f_dret;
				ed._arrel->f_dret = p->f_dret;
				nou = expressio(buida, token(token::EXPONENCIACIO), ed)._arrel;
				nou->f_esq = p->f_esq->f_esq;
			}
			// 7. f) (exp(E))ˆE′ → exp(E ∗ E′)
			else if (p->f_esq->info.tipus() == token::EXP) {
				expressio ed = expressio(buida, token(token::MULTIPLICACIO), buida);
				ed._arrel->f_esq = p->f_esq->f_dret;
				ed._arrel->f_dret = p->f_dret;
				nou = expressio(buida, token(token::EXP), ed)._arrel;
			}
		}
		// 8. Regles de radicació:
		else if (p->info.tipus() == token::SQRT) {
			// 8. d) sqrt(E) → E ˆ 1/2
			if (p->f_dret->info.tipus() != token::CT_REAL) {
				if (p->f_dret->info.tipus() == token::CT_ENTERA) {
					// 8. a) sqrt(c) → error, per qualsevol constant entera, racional o real c < 0
					if (p->f_dret->info.valor_enter() < 0) {
						throw error(SqrtDeNegatiu);
						esborra_nodes(p);
					}
					// 8. b) sqrt(0) → 0
					else if(p->f_dret->info.valor_enter() == 0) {
						nou = expressio(token(0))._arrel;
						esborra_nodes(p);
					}
					else {
						nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
						nou->f_esq = p->f_dret;
						racional ra(1, 2);
						token aux(ra);
						nou->f_dret = expressio(aux)._arrel;
					}
				}
				else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
					// 8. a) sqrt(c) → error, per qualsevol constant entera, racional o real c < 0
					if (p->f_dret->info.valor_racional().num() < 0) {
						throw error(SqrtDeNegatiu);
						esborra_nodes(p);
					}
					// 8. b) sqrt(0) → 0
					else if (p->f_dret->info.valor_racional().num() == 0) {
						nou = expressio(token(0))._arrel;
						esborra_nodes(p);
					}
					else {
						nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
						nou->f_esq = p->f_dret;
						racional ra(1, 2);
						token aux(ra);
						nou->f_dret = expressio(aux)._arrel;
					}
				}
				else {
					nou = expressio(buida, token(token::EXPONENCIACIO), buida)._arrel;
					nou->f_esq = p->f_dret;
					racional ra(1, 2);
					token aux(ra);
					nou->f_dret = expressio(aux)._arrel;
				}
			}
			else {
				// 8. a) sqrt(c) → error, per qualsevol constant entera, racional o real c < 0
				if (p->f_dret->info.valor_real() < 0) {
					throw error(SqrtDeNegatiu);
					esborra_nodes(p);
				}
				// 8. b) sqrt(0) → 0
				else if (p->f_dret->info.valor_real() == 0) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
				/*8. c) Si la funció sqrt està aplicada sobre una constant en coma flotant es calcula
						d’immediat el seu resultat usant la funció corresponent de la biblioteca mate-
						màtica de C++, i en concret es pendrà el valor positiu (p.e. tant 3.0 com -3.0
						són valors correctes per sqrt(9.0), però ens quedarem amb el primer).*/ 
				else {
					double resultat = sqrt(p->f_dret->info.valor_real());
					token res(resultat);
					nou = expressio(res)._arrel;
				}
			}
		}
		// 9. Regles de les funcions exponencial i logaritme (natural):
		else if (p->info.tipus() == token::EXP) {
			if (p->f_dret->info.tipus()== token::CT_ENTERA) {
				// 9. a) exp(0) → 1
				if (p->f_dret->info.valor_enter() == 0) {
					nou = expressio(token(1))._arrel;
					esborra_nodes(p);
				}
				// 9. b) exp(1) → e
				else if (p->f_dret->info.valor_enter() == 1) {
					nou = expressio(token(token::CT_E))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				// 9. a) exp(0) → 1
				if (p->f_dret->info.valor_racional().num() == 0) {
					nou = expressio(token(1))._arrel;
					esborra_nodes(p);
				}
				// 9. b) exp(1) → e
				else if ((p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) == 1) {
					nou = expressio(token(token::CT_E))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL) {
				// 9. a) exp(0) → 1
				if (p->f_dret->info.valor_real() == 0) {
					nou = expressio(token(1))._arrel;
					esborra_nodes(p);
				}
				// 9. b) exp(1) → e
				else if (p->f_dret->info.valor_real() == 1) {
					nou = expressio(token(token::CT_E))._arrel;
					esborra_nodes(p);
				}
				/* 9. k) Si la funció exp o log estan aplicades sobre una constant en coma flotant
						 es calcula d’immediato el seu resultat usant les funcions corresponents de la
						 biblioteca matemàtica de C++.*/
				else  {
					double resultat = exp(p->f_dret->info.valor_real());
					nou = expressio(token(resultat))._arrel;
					esborra_nodes(p);
				}
			}
			// 9. c) exp(log(E)) → E
			else if (p->f_dret->info.tipus() == token::LOG) {
				nou = p->f_dret->f_dret;
			}
		}
		else if (p->info.tipus() == token::LOG) {
			if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				// 9. d) log(c) → error, per qualsevol constant entera, racional o real c ≤ 0
				if (p->f_dret->info.valor_enter() <= 0) {
					throw error(LogDeNoPositiu);
					esborra_nodes(p);
				}
				// 9. e) log(1) → 0
				else if (p->f_dret->info.valor_enter() == 1) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				// 9. d) log(c) → error, per qualsevol constant entera, racional o real c ≤ 0
				if (p->f_dret->info.valor_racional().num() <= 0) {
					throw error(LogDeNoPositiu);
					esborra_nodes(p);
				}
				// 9. e) log(1) → 0
				else if ((p->f_dret->info.valor_racional().num() / p->f_dret->info.valor_racional().denom()) == 1) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
			}
			else if (p->f_dret->info.tipus() == token::CT_REAL) {
				// 9. d) log(c) → error, per qualsevol constant entera, racional o real c ≤ 0
				if (p->f_dret->info.valor_real() <= 0) {
					throw error(LogDeNoPositiu);
					esborra_nodes(p);
				}
				// 9. e) log(1) → 0
				else if (p->f_dret->info.valor_real() == 1) {
					nou = expressio(token(0))._arrel;
					esborra_nodes(p);
				}
				/* 9. k) Si la funció exp o log estan aplicades sobre una constant en coma flotant
						 es calcula d’immediato el seu resultat usant les funcions corresponents de la
						 biblioteca matemàtica de C++.*/
				else  {
					double resultat = log(p->f_dret->info.valor_real());
					nou = expressio(token(resultat))._arrel;
					esborra_nodes(p);
				}
			}
			// 9. f) log(e) → 1
			else if (p->f_dret->info.tipus() == token::CT_E) {
				nou = expressio(token(1))._arrel;
					esborra_nodes(p);
			}
			// 9. g) log(exp(E)) → E
			else if (p->f_dret->info.tipus() == token::EXP) {
				nou = p->f_dret->f_dret;
			}
			// 9. h) log(E ∗ E′)) → log(E) + log(E′)
			else if (p->f_dret->info.tipus() == token::MULTIPLICACIO) {
				expressio ee = expressio(buida, token(token::LOG), buida);
				ee._arrel->f_dret = p->f_dret->f_esq;
				expressio ed = expressio(buida, token(token::LOG), buida);
				ed._arrel->f_dret = p->f_dret->f_dret;
				nou = expressio(buida, token(token::SUMA), buida)._arrel;
				nou->f_esq = ee._arrel;
				nou->f_dret = ed._arrel;		
			}
			// 9. i) log(E/E′)) → log(E) − log(E′)
			else if (p->f_dret->info.tipus() == token::DIVISIO) {
				expressio ee = expressio(buida, token(token::LOG), buida);
				ee._arrel->f_dret = p->f_dret->f_esq;
				expressio ed = expressio(buida, token(token::LOG), buida);
				ed._arrel->f_dret = p->f_dret->f_dret;
				nou = expressio(buida, token(token::RESTA), buida)._arrel;
				nou->f_esq = ee._arrel;
				nou->f_dret = ed._arrel;
			}
			// 9. j) log(EˆE′)) → E′ ∗ log(E)
			else if (p->f_dret->info.tipus() == token::EXPONENCIACIO) {
				expressio ed(buida, token(token::LOG), buida);
				ed._arrel->f_dret = p->f_dret->f_esq;
				nou = expressio(buida, token(token::MULTIPLICACIO), buida)._arrel;
				nou->f_esq = p->f_dret->f_dret;
				nou->f_dret = ed._arrel;
			}
		}
		// 10. Operació evalf:
		else if(p->info.tipus() == token::EVALF) {
			// 10. a) Si s’aplica sobre una constant en coma flotant torna la pròpia constant.
			if (p->f_dret->info.tipus() == token::CT_REAL) {
				nou = expressio(token(p->f_dret->info.valor_real()))._arrel; 
				esborra_nodes(p);
			}
			/* 10. b) Si s’aplica sobre un enter realitza la conversió, i si s’aplica sobre un racional
					  es calcula el quocient real entre el numerador i el denominador.*/
			else if (p->f_dret->info.tipus() == token::CT_ENTERA) {
				nou = expressio(token((double) p->f_dret->info.valor_enter()))._arrel;
				esborra_nodes(p);
			}
			/* 10. b) Si s’aplica sobre un enter realitza la conversió, i si s’aplica sobre un racional
					  es calcula el quocient real entre el numerador i el denominador.*/ 
			else if (p->f_dret->info.tipus() == token::CT_RACIONAL) {
				nou = expressio(token((p->f_dret->info.valor_racional().num()) / (p->f_dret->info.valor_racional().denom())))._arrel; 
				esborra_nodes(p);
			}
			// 10. c) Si s’aplica sobre la constant e retorna el valor 2.718281828.
			else if (p->f_dret->info.tipus() == token::CT_E) {
				nou = expressio(token(2.718281828))._arrel;
				esborra_nodes(p);
			}
			
			/* 10. d) Si s’aplica sobre una variable x que no té valor assignat retorna la pròpia
					  variable.*/
			else if (p->f_dret->info.tipus() == token::VARIABLE) {
				nou = expressio(token(p->f_dret->info.identificador_variable()))._arrel;
				esborra_nodes(p);
			}
			else if (p->f_dret->info.tipus() == token::VAR_PERCENTATGE) {
				nou = expressio(token(token::VAR_PERCENTATGE))._arrel;
				esborra_nodes(p);
			}
			/* 10. e) Per la resta, la seva aplicació és recursiva: evalf(E op E′) → evalf(E) op evalf(E′)
					  si op és una operació binària i evalf(f (E)) → f (evalf(E)) per
					  una funció f qualsevol exceptuant la pròpia evalf.*/
			else if (check_op(p->f_dret->info.tipus())) {
				if (es_operador_binari(p->f_dret->info)) {
					expressio ed(buida, token(token::EVALF), buida);
					ed._arrel->f_dret = p->f_dret->f_dret;
					expressio ee(buida, token(token::EVALF), buida);
					ee._arrel->f_dret = p->f_dret->f_esq;
					nou = expressio(buida, token(p->f_dret->info), buida)._arrel;
					nou->f_dret = ed._arrel;
					nou->f_esq = ee._arrel;
					nou = simplify_subtree(nou);
				}
				else if (es_operador_unari(p->f_dret->info)) {
					expressio ed(buida, token(token::EVALF), buida);
					ed._arrel->f_dret = p->f_dret->f_dret;
					nou = expressio(buida, token(p->f_dret->info), buida)._arrel;
					nou->f_dret = ed._arrel;
					nou = simplify_subtree(nou);
				}
			}
		}
		p = nou;
	}
	return p;
};

/*	Metode recursiu auxiliar per aplicar simplificacions matematiques sobre una expressio
	en un pas.	Es recorre l'arbre en postordre.	*/
expressio::node* expressio::simplify_one_step(bool & simplificat, node *p) 
//PRE: p apunta a un node del subarbre d'una expressio al qual es volen fer les simplificacions.	
//POST: Retorna el punter que apunta al subarbre simplificat.
{
	if (p != NULL) {
		p->f_esq = simplify_one_step(simplificat, p->f_esq);
		
		if (simplificat == false) {
			p->f_dret = simplify_one_step(simplificat, p->f_dret);
		}
		if (simplificat == false) {
			node *i(p);
			p = simplify_subtree(p);
			if(arbresIguals(p, i))
				simplificat = false;
			else
				simplificat = true;
		}
	}
	return p;
};

/*	Metode per simplificar una expressio. Es fa el recorregut en postordre.	*/
expressio::node* expressio::simplify(node *p) 
//PRE: p apunta a un node d'una expressio.	
//POST: Retorna la expressio completament simplificada.
{
	if (p != NULL) {
		node *i(p);
		p->f_esq = simplify(p->f_esq);
		p->f_dret = simplify(p->f_dret);	
		p = simplify_subtree(p);
		if (not arbresIguals(p, i)) {
			p = simplify(p);
		}
	}
	return p;
};

/*	Metode auxiliar per comprovar l'associativitat entre operadors amb mateixa precedencia.	*/
bool expressio::aso_esq_dre(token top, token it)
//PRE: True
//POST:	Retorna true si es compleix l'associativitat esquerra-dreta entre operadors
//		amb mateixa precedencia.
{
	bool asociatiu = false;
	if (top.tipus() == token::SUMA and it.tipus() == token::RESTA) {
		asociatiu = true;
	}
	else if (top.tipus() == token::SUMA and it.tipus() == token::SUMA) {
		asociatiu = true;
	}
	else if (top.tipus() == token::RESTA and it.tipus() == token::RESTA) {
		asociatiu = true;
	}
	else if (top.tipus() == token::MULTIPLICACIO and it.tipus() == token::DIVISIO) {
		asociatiu = true;
	}
	else if (top.tipus() == token::MULTIPLICACIO and it.tipus() == token::MULTIPLICACIO) {
		asociatiu = true;
	}
	else if (top.tipus() == token::DIVISIO and it.tipus() == token::DIVISIO) {
		asociatiu = true;
	}
	else if (top.tipus() == token::DIVISIO and it.tipus() == token::MULTIPLICACIO) {
		asociatiu = true;
	}

	return asociatiu;
};

//-----------Metodes publics:

/* Constructora d'una expressió formada per un sol token: un operand. Si
 s'utiliza el valor del token per defecte es construeix la que
 anomenem "expressió buida". Si el tipus del token no és el del token
 per defecte (NULLTOK), ni el d'una CT_ENTERA, CT_RACIONAL, CT_REAL,
 CT_E, VARIABLE o VAR_PERCENTAtGE es produeix un error sintàctic. */
expressio::expressio(const token t) throw(error)
{
	if (check_op(t.tipus())) {
		throw error(ErrorSintactic);
	}
	else {
		node *p = new node;
		p->info = t;
		p->f_esq = NULL;
		p->f_dret = NULL;

		_arrel = p;
	}
}


/* Constructora a partir d'una seqüència de tokens. Es produeix un error si
	la seqüència és buida o si no es pot construir l'arbre d'expressió
	corresponent(és a dir, si és sintàcticament incorrecta). */
expressio::expressio(const list<token> &l) throw(error)
{
	bool es_valid = true;
	stack<expressio> expr;
	stack<token> op;

	if (l.empty()) {
		es_valid = false;
	}
	else 
	{
		for (list<token>::const_iterator it = l.begin(); it != l.end() and es_valid; ++it) {
			if (consecutius_valids(it, l)) {
				if (check_op((*it).tipus())) {
					if (es_operador_unari(*it)) {
						op.push(*it);
					}
					else if (es_operador_binari(*it)) {
						if (op.empty()) {
							op.push(*it);
						}
						else { 
							while (not op.empty() and op.top().tipus() != token::OBRIR_PAR ) { // Per modificar la pila per precedencia.
								if (op.top() > (*it)) {
									expressio ed = expr.top();
									expr.pop();
									if (es_operador_unari(op.top())) {
										expressio buida = expressio();
										expressio nou(buida, op.top(), ed);
										expr.push(nou);
										op.pop();
									}
									else if (es_operador_binari(op.top())) {
										expressio ee = expr.top();
										expr.pop();
										expressio nou(ee, op.top(), ed);
										expr.push(nou);
										op.pop();
									}
								}
								else if (aso_esq_dre(op.top(), (*it))) {
									expressio ed = expr.top();
									expr.pop();
									if (es_operador_unari(op.top())) {
										expressio buida = expressio();
										expressio nou(buida, op.top(), ed);
										expr.push(nou);
										op.pop();
									}
									else if (es_operador_binari(op.top())) {
										expressio ee = expr.top();
										expr.pop();
										expressio nou(ee, op.top(), ed);
										expr.push(nou);
										op.pop();
									}
								}
								else {
									break;
								}
							}
							op.push(*it);
						}
					}
					else { // Es parentesi obert
						op.push((*it));
					}
				}
				else if (check_operand((*it).tipus())) {
					if ((*it).tipus() == token::CT_RACIONAL) {
						if ((*it).valor_racional().denom() == 1) {
							token nou((*it).valor_racional().num());
							expressio nova(nou);
							expr.push(nova);
						}
						else {
							expressio nou(*it);
							expr.push(nou);
						}
					}
					else {
						expressio nou(*it);
						expr.push(nou);
					}
				}
				else {
					bool trobat = false;
					if (not trobat and (*it).tipus() == token::TANCAR_PAR) {
						if (not op.empty()) {
							while (not op.empty() and op.top().tipus() != token::OBRIR_PAR) {
								expressio ed = expr.top();
								expr.pop();
								if (es_operador_unari(op.top())) { // Es operador unari:
									expressio buida = expressio();
									expressio nou(buida, op.top(), ed);
									expr.push(nou);
								}
								else if (es_operador_binari(op.top())) { // Es operador binari:
									expressio ee = expr.top();
									expr.pop();
									expressio nou(ee, op.top(), ed);
									expr.push(nou);
								}
								op.pop();
							}
							op.pop(); // pop de "(".
						}
						else {
							es_valid = false;
						}
					}
				}
			}
			else {
				es_valid = false;
			}
		}

		while (es_valid and not op.empty()) {
			if (op.top().tipus() != token::OBRIR_PAR) {
				expressio ed = expr.top();
				expr.pop();
				if (es_operador_unari(op.top())) { // Es operador unari:
					expressio buida = expressio();
					expressio nou(buida, op.top(), ed);
					expr.push(nou);
				}
				if (es_operador_binari(op.top())) { // Es operador binari:
					expressio ee = expr.top();
					expr.pop();
					expressio nou(ee, op.top(), ed);
					expr.push(nou);
				}
				op.pop();
			}
			else {
				es_valid = false;
			}
		}

	}

	if (es_valid) {
		(*this) = expr.top();
		// imprimir_expressio(*this);
	}
	else {
		throw error(ErrorSintactic);
	}
}

// Constructora per còpia, assignació i destructora.
expressio::expressio(const expressio &e) throw(error)
{
	_arrel = copia_nodes(e._arrel);
}

expressio &expressio::operator=(const expressio &e) throw(error)
{
	if (this != &e) {
		node *aux;
		aux = copia_nodes(e._arrel);
		_arrel = aux;
	}

	return (*this);
}
expressio::~expressio() throw(error)
{
}

// Retorna cert si i només si s'aplica a l'expressió buida.
expressio::operator bool() const throw()
{
	bool b = false;
	b = (_arrel->info.tipus() == token::NULLTOK);
	return b;
}

/* Operadors d'igualtat i desigualtat. Dues expressions es consideren
 iguals si i només si els seus arbres d'expressió són idèntics. */
bool expressio::operator==(const expressio &e) const throw()
{
	bool b = arbresIguals(_arrel, e._arrel);

	return b;
}

bool expressio::operator!=(const expressio &e) const throw()
{
	return not(*this == e);
}

/* Retorna una llista sense repeticions, en qualsevol ordre, amb
 els noms de les variables de l'expressió. */
void expressio::vars(list<string> &l) const throw(error)
{
	rec_preordre(_arrel, l);
}

/* Substitueix totes les aparicions de la variable de nom v per
 l'expressió e. Si no existeix la variable v dins de l'expressió a la
 que apliquem aquest mètode l'expressió no es modifica. */
void expressio::apply_substitution(const string &v, const expressio &e) throw(error)
{
	list<string> l;
	vars(l);

	bool trobat = false;

	list<string>::iterator it;
	for (it = l.begin(); it != l.end(); ++it) {
		trobat = (*it == v);
		if (trobat)
			break;
	}
	if (trobat) { // Modifiquem l'arbre
		_arrel = cerca(_arrel, v, e);
	}
}

/* Aplica un pas de simplificació a l'expressió. La subexpressió a
 simplificar es busca seguint el recorregut "left to right" postordre
 explicat a l'apartat "Procés d'avaluació". Es pot produir qualsevol dels
 errors semàntics que apareixen més avall numerats des del 32 al 35. */
void expressio::simplify_one_step() throw(error)
{
	bool simplificat = false;
	_arrel = simplify_one_step(simplificat, _arrel);
	//imprimir_expressio((*this));
}

/* Aplica successius passos de simplificació com l'anterior fins que
 l'expressió es trobi completament simplificada. Llavors diem que es
 troba en "forma normal". Es pot produir qualsevol dels errors
 semàntics que apareixen més avall numerats des del 32 al 35. */
void expressio::simplify() throw(error)
{
	_arrel = simplify(_arrel);
}

/* Converteix l'expressió en la seqüència de tokens lt corresponent: els
 operadors apareixen entre els operands si són infixos, abans si són
 prefixos i els símbols de funció van seguits de parèntesis que tanquen
 els seus arguments separats per comes. S'han d'introduir només aquells
 parèntesis que siguin estrictament necessaris per trencar les regles de
 precedència o associativitat en l'ordre d'aplicació dels operadors. */
void expressio::list_of_tokens(list<token> &lt) throw(error)
{
	list_of_tokens_aux(_arrel, lt);
}