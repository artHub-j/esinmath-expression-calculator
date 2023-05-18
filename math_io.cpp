#include "math_io.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>

using std::isdigit;
using std::isalpha;
using std::sprintf;
using std::atoi;
using std::atof;

typedef string::size_type string_size;


bool check_sign (token::codi cod) {
  return cod == token::SUMA or cod == token::RESTA or
         cod == token::MULTIPLICACIO or cod == token::DIVISIO or cod == token::EXPONENCIACIO or
         cod == token::CANVI_DE_SIGNE or cod == token::SIGNE_POSITIU or
         cod == token::OBRIR_PAR or cod == token::COMA;
}

bool check_reserved (list<token>& lt, string& id) {
  if(id == "e") {
    lt.push_back(token(token::CT_E));
    return true;
  }
  if(id == "log") {
    lt.push_back(token(token::LOG));
    return true;
  }
  if(id == "sqrt") {
    lt.push_back(token(token::SQRT));
    return true;
  }
  if(id == "exp") {
    lt.push_back(token(token::EXP));
    return true;
  }
  if(id == "evalf") {
    lt.push_back(token(token::EVALF));
    return true;
  }
  if(id == "unassign") {
    lt.push_back(token(token::DESASSIGNACIO));
    return true;
  }
  if(id == "byebye") {
    lt.push_back(token(token::BYEBYE));
    return true;
  }
  return false;
}

string_size skip_digits (const string& s, string_size i) {
  string_size j = i;
  while (j != s.size() and isdigit(s[j])) ++j;
  return j;
}

void alfabetica (const string& s, string_size& i, list<token>& lt) {
  string ident;
  string_size j = i;
  while (j != s.size() and (isalpha(s[j]) or s[j] == '_')) ++j;
  ident = s.substr(i, j - i);
  if (!check_reserved(lt, ident)) lt.push_back(token(ident));
  i = j;
}

void exponencial (const string& s, string_size&i, string_size&j,
                  list<token>& lt, bool decimal=false) throw(error) {
  bool sign = false;
  if (++j == s.size()) {
    --j;
    if (decimal) lt.push_back(token(atof(s.substr(i, j - i).c_str())));
    else {
      lt.push_back(token(atoi(s.substr(i, j - i).c_str())));
    }
  } else {
    if (s[j] == '+' or s[j] == '-') {
      sign = true;
      ++j;
    }
    if (j == s.size() or !isdigit(s[j])) {
      if (sign) j -= 2;
      else --j;
      if (decimal) lt.push_back(token(atof(s.substr(i, j - i).c_str())));
      else lt.push_back(token(atoi(s.substr(i, j - i).c_str())));
    } else {
      j = skip_digits(s, j);
      lt.push_back(token(atof(s.substr(i, j - i).c_str())));
    }
  }
}

void numerica (const string& s, string_size& i, list<token>& lt) throw(error) {
  string_size j = skip_digits(s, i);
  int num = atoi(s.substr(i, j - i).c_str());
  if (j == s.size()) lt.push_back(token(num));
  else {
    switch (s[j]) {
    case '.': // real
      if (++j == s.size() or !isdigit(s[j]))
        throw error(math_io::ErrorLexic);
      j = skip_digits(s, j);
      if (j != s.size() and s[j] == 'E')
        exponencial(s,i,j,lt,true);
      else
        lt.push_back(token(atof(s.substr(i, j - i).c_str())));
      break;
    case 'E': // exponent
      exponencial(s,i,j,lt);
      break;
    default: // enter
      lt.push_back(token(atoi(s.substr(i, j - i).c_str())));
    }
  }
  i = j;
}

void assignacio (const string& s, string_size& i, list<token>& lt) throw(error) {
  if (++i == s.size() or s[i++] != '=')
    throw error(math_io::ErrorLexic);
  lt.push_back(token(token::ASSIGNACIO));
}

void altres (const string& s, string_size& i, list<token>& lt) throw(error) {
  switch (s[i++]) {
  case '+':
    if (lt.empty() or check_sign(lt.back().tipus()))
      lt.push_back(token(token::SIGNE_POSITIU));
    else lt.push_back(token(token::SUMA));
    break;
  case '-':
    if (lt.empty() or check_sign(lt.back().tipus()))
      lt.push_back(token(token::CANVI_DE_SIGNE));
    else lt.push_back(token(token::RESTA));
    break;
  case '*':
    lt.push_back(token(token::MULTIPLICACIO));
    break;
  case '/':
    lt.push_back(token(token::DIVISIO));
    break;
  case '^':
    lt.push_back(token(token::EXPONENCIACIO));
    break;
  case '(':
    lt.push_back(token(token::OBRIR_PAR));
    break;
  case ')':
    lt.push_back(token(token::TANCAR_PAR));
    break;
  case ',':
    lt.push_back(token(token::COMA));
    break;
  case '%':
    lt.push_back(token(token::VAR_PERCENTATGE));
    break;
  default: //caracters erronis
    throw error(math_io::ErrorLexic);
  }
}

void math_io::scan (const string& s, list<token>& lt) throw(error) {
  string_size i;
  lt.clear();
  i = 0;
  while (i != s.size() and isspace(s[i])) ++i;
  while (i != s.size()) {
    if (isalpha(s[i]) or s[i] == '_') // identificador, paraula reservada
      alfabetica(s, i, lt);
    else if (isdigit(s[i])) // enter, real
      numerica(s, i, lt);
    else if (s[i] == ':') // assignacio
      assignacio(s, i, lt);
    else // altres
      altres(s, i, lt);
    while (i != s.size() and isspace(s[i])) ++i;
  }
}

string math_io::tostring (const list<token> & lt) {
  char buf[20];
  string s_exp, s_tok, s;
  for (list<token>::const_iterator it = lt.begin(); it != lt.end(); ++it) {
    switch(it->tipus()) {
    case token::NULLTOK:
      break;
    case token::CT_ENTERA:
      sprintf(buf, "%i", it->valor_enter());
      s_tok = string(buf);
      break;
    case token::CT_RACIONAL:
      sprintf(buf, "%i", it->valor_racional().num());
      s = string(buf);
      sprintf(buf, "%i", it->valor_racional().denom());
      s_tok = s + "/" + string(buf);
      break;
    case token::CT_REAL:
      sprintf(buf, "%f", it->valor_real());
      s_tok = string(buf);
      break;
    case token::CT_E:
      s_tok = "e";
      break;
    case token::VARIABLE:
      s_tok = it->identificador_variable();
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
  }
  return s_exp;
}
