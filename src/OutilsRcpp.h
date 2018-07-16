#pragma once

#include <Rcpp.h>
#include <string>
#include <deque>
#include <map>
#include <array>
#include <memory>
class Rdout;
void export_df(Rcpp::Environment& env, Rdout& df);

class Context {
public:
  static Rcpp::Environment env;
  static Rcpp::List lst;
  static const char* lst_name;
  static int iter;
  enum {NOCHECK,CHECK_FALSE,CHECK_TRUE,CHECK_OPTION};
  static int check;
  static std::deque<Rdout*> list_df;
private: //TODO
  Rcpp::Environment old_env;
  Rcpp::List old_lst;
  const char* old_lst_name;
  int old_iter;
  bool old_check;
public:
  Context(Rcpp::Environment env) :
    old_env(Context::env),
    old_lst(Context::lst),
    old_lst_name(Context::lst_name),
    old_iter(Context::iter),
    old_check(Context::check)
  {
    Context::env = env;
  }
  Context(Rcpp::Environment env,const char* lst_name) :
    old_env(Context::env),
    old_lst(Context::lst),
    old_lst_name(Context::lst_name),
    old_iter(Context::iter),
    old_check(Context::check)
  {
    Context::env = env;
    Context::lst_name = lst_name;
    Context::check = NOCHECK;
    Context::iter = 0;
    if(Context::env.exists(lst_name)) {
      Context::lst = env[lst_name];
    } else {
      Rcpp::Rcerr << "La liste '" << lst_name << "' est absente de l'environnement" << std::endl;
      ::Rf_error("parametres manquants");
    }
  }
  Context(Rcpp::Environment env,const char* lst_name,bool iter) :
    old_env(Context::env),
    old_lst(Context::lst),
    old_lst_name(Context::lst_name),
    old_iter(Context::iter),
    old_check(Context::check)
  {
    Context::env = env;
    Context::lst_name = lst_name;
    Context::check = (iter == true) ? CHECK_FALSE : NOCHECK;
    Context::iter = 0;
    if(Context::env.exists(lst_name)) {
      Context::lst = env[lst_name];
    } else {
      Rcpp::Rcerr << "La liste '" << lst_name << "' est absente de l'environnement" << std::endl;
      ::Rf_error("parametres manquants");
    }
  }
  void Next() {
    Context::iter++;
    Context::check = Context::CHECK_TRUE;
  }
  static void add_df(Rdout* df) {
    Context::list_df.push_back(df);
  }
  ~Context() {
    if(list_df.size())
      for(auto& df : Context::list_df)
        if(df != NULL) export_df(env,*df);

    Context::env = old_env;
    Context::lst_name = old_lst_name;
    Context::lst = old_lst;
    Context::check = old_check;
    Context::iter = old_iter;
  }
};


class Levels {
public:
  Levels(std::vector<int> levels, const char* labels) :
    m_levels(levels), m_labels(make_labels(labels))
    {
    }
  
  Levels(std::vector<std::string> labels) :
    m_levels(labels.size()),
    m_labels(labels)
    {
      for(unsigned int i = 0; i < m_levels.size(); i++) {
        m_levels[i] = i;
      }
    }
  const std::vector<int>& get_levels() const {
    return m_levels;
  }
  const std::vector<std::string>& get_labels() const {
    return m_labels;
  }
  const unsigned int size() const {
    return m_levels.size();
  }
private:
  static std::vector<std::string> make_labels(const char* labels) {
  	std::vector<std::string> v_labels;
  	const char* old = labels;
  	const char* x = labels;
  	for (; *x != '\0'; x++) {
  		if (*x == ',' || *x == 0) { v_labels.emplace_back(old, x - old); old = x + 1; }
  	}
  	v_labels.emplace_back(old, x - old);
  	return v_labels;
  }
private:
  std::vector<int> m_levels;
  const std::vector<std::string> m_labels;
};

/*
class Factor {
  const Levels levels;
  const char* ch;
public:
  Factor(const char* ch, Levels levels ) : ch(ch), levels(levels) {}
};*/

class Factor : public Rcpp::IntegerVector {};

class ValDef {
public:
  virtual double get() {return 0.0;}
};
template<typename T>
class ValDefImpl : public ValDef {
public:
  T val;
  ValDefImpl(T val) : val(val) {}
  double get() {return (double)val;};
};
  
class Rinput {
  using uint = unsigned int;
private:
  Rcpp::RObject m_index;
  const char* m_name;
  bool ok_factor = false;
  std::shared_ptr<ValDef> m_default = nullptr;
public:
  Rinput(const char* name) {
    m_name = name;
  }
  template<typename T>
  Rinput(const char* name, T defaut) {
    m_name = name; m_default = std::make_shared<ValDef>(ValDefImpl<T>(defaut));
  }
  Rinput(const char* name, Levels levels) {
    m_name = name;
    if(Context::check != Context::CHECK_TRUE ) {
      try {
        if(!Context::lst.containsElementNamed(m_name)) throw std::logic_error("manquant");
        else if(!Rcpp::is<Rcpp::IntegerVector>(Context::lst[m_name])) throw std::logic_error("n'est pas un factor");
        else {
          Rcpp::IntegerVector tmp = Rcpp::as<Rcpp::IntegerVector>(Context::lst[m_name]);
          if(tmp.attributeNames().size() == 0) throw std::logic_error("n'est pas un factor");
          if(!Rcpp::is<Rcpp::CharacterVector>(tmp.attr("class"))) throw std::logic_error("n'est pas un factor");
          if(!Rcpp::is<Rcpp::CharacterVector>(tmp.attr("class"))) throw std::logic_error("test");
          if(!Rcpp::is<Rcpp::CharacterVector>(tmp.attr("levels"))) throw std::logic_error("n'est pas un factor2");
          if(Rcpp::as<std::string>(Rcpp::as<Rcpp::CharacterVector>(tmp.attr("class"))[0]) != "factor" && 
            Rcpp::as<std::string>(Rcpp::as<Rcpp::CharacterVector>(tmp.attr("class"))[0]) != "ordered") throw std::logic_error("Not a factor");
          if(Rcpp::as<Rcpp::CharacterVector>(tmp.attr("levels")).size() != (int)levels.size() ) throw std::logic_error("Insuffisent number of levels");
          else {
            Rcpp::CharacterVector labels = Rcpp::as<Rcpp::CharacterVector>(tmp.attr("levels"));
            for(unsigned int i = 0; i < levels.size(); i++) {
              if(Rcpp::as<std::string>(labels[i]) != levels.get_labels()[i]) {
                Rcpp::Rcout << "Erreur, label attendu, label fourni : " << std::endl;
                for(unsigned int i = 0; i < levels.size(); i++) {
                  Rcpp::Rcout << Rcpp::as<std::string>(labels[i]) << " " << levels.get_labels()[i] << std::endl;
                }
                throw std::logic_error("Labels incorrects ou non ordonnés");
              }
            }
          }
        }
      }
      catch(const std::exception& err) {
        Rcpp::Rcerr << "Erreur objet '" << name << "', liste " << Context::lst_name << " : " << err.what() << std::endl;
        ::Rf_error("parametre incorrect");
      }
      catch(...) {
        Rcpp::Rcerr << "Erreur objet '" << name << "', liste " << Context::lst_name << " : " << std::endl;
        ::Rf_error("parametre incorrect");
      }
    } // end !Context::Check_TRUE
  }

  // "un scalaire du type integer."
  template<typename T>
  T get(const char* type) {
    if(Context::check != Context::CHECK_TRUE && m_default == nullptr) {
      try {
        m_index = Context::lst[m_name];
      } catch(...) {
        Rcpp::Rcerr << "L'objet '" << m_name << "' est absent de la liste '" <<  Context::lst_name << "'." << std::endl;
        ::Rf_error("parametres manquants");
      }
      if(Rcpp::is<T>(m_index)) {
          return Rcpp::as<T>(m_index);
      } else {
        Rcpp::Rcerr << "L'objet '" << m_name << "' de la liste '" <<  Context::lst_name  << "' n'est pas " << type << "." << std::endl;
        ::Rf_error("parametre incorrect");
      }
    }
    else if(m_default == nullptr) {
      m_index = Context::lst[m_name];
      return Rcpp::as<T>(m_index);
    } else {
      try {
        m_index = Context::lst[m_name];
        return Rcpp::as<T>(m_index);
      } 
      catch(...) {
        return T(m_default->get());
      }
    }
    return T();
  }

  operator int() {
    if(Context::check == Context::NOCHECK)  return get<int>("un scalaire du type integer");
    else return get<Rcpp::IntegerVector>("un vecteur du type integer")[Context::iter];
    }
  operator double() {
    if(Context::check == Context::NOCHECK)  return get<double>("un scalaire du type numeric");
    else return get<Rcpp::NumericVector>("un vecteur du type numeric")[Context::iter];
    }
  operator Rcpp::NumericVector() {return get<Rcpp::NumericVector>("un vecteur du type numeric");}
  operator Rcpp::IntegerVector() {return get<Rcpp::IntegerVector>("un vecteur du type integer");}
  operator Rcpp::CharacterVector() {return get<Rcpp::CharacterVector>("un vecteur du type character");}
  operator std::vector<std::string>() {return Rcpp::as<std::vector<std::string>>(get<Rcpp::CharacterVector>("un vecteur du type character"));}
  operator bool() {
    try {
      Rcpp::LogicalVector tmp = Rcpp::as<Rcpp::LogicalVector>(Context::lst[m_name]);
      if(tmp.size() == 1 && tmp[0] == true)
        return true;
    } catch(...) {
      for(int i = 0; i < Context::lst.size(); i++)
        if(Rcpp::is<Rcpp::String>(Context::lst[i]))
          if(Rcpp::as<Rcpp::String>(Context::lst[i]) ==  m_name) {
            Rcpp::Rcout << "Option : " << m_name << std::endl;
            return true;
          }
      return false;
    }
    return false;
  }
};

template<typename T>
inline T Rdin(const char* name) {
  auto c = Context(Context::env,name);
  T tmp = T();
  return tmp;
}

template<typename T>
inline std::vector<T> Rdin(const char* name, const char* strate1, Levels levels1) {
  auto c = Context(Context::env,name,true);
  Rcpp::IntegerVector factor1 = Rinput(strate1,levels1);
  if(factor1.size() != (int)levels1.size()) {
    Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' comporte des doublons" << std::endl;
    ::Rf_error("parametre incorrect");
  }

  std::vector<T> tmp;
  int i = 0;
  for(auto l : levels1.get_levels()) {
    if(factor1[i]!=l+1) {
      Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' n'est pas trié correctement." << factor1[i-1] << " " << l+1 << std::endl;
      ::Rf_error("parametre incorrect");
    }
    tmp.emplace_back();
    c.Next();
    i++;
  }
  return tmp;
}


template<typename T>
inline std::vector<std::vector<T>>
  Rdin(
    const char* name,
    const char* strate1, Levels levels1,
    const char* strate2, Levels levels2) {
  auto c = Context(Context::env,name,true);
  Rcpp::IntegerVector factor1 = Rinput(strate1,levels1);
  Rcpp::IntegerVector factor2 = Rinput(strate2,levels2);
  if(factor1.size() != levels1.size() * levels2.size()) {
    Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' comporte des doublons" << std::endl;
    ::Rf_error("parametre incorrect");
  }

  std::vector<std::vector<T>> tmp;
  int i = 0;
  for(auto l1 : levels1.get_levels()) {
    tmp.emplace_back();
    for(auto l2 : levels2.get_levels()) {
      if(factor1[i]!=l1+1 || factor2[i]!=l2+1) {
        Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' n'est pas trié correctement." << std::endl;
        ::Rf_error("parametre incorrect");
      }
      tmp.back().emplace_back();
      c.Next();
      i++;
    }
  }
  return tmp;
}


template<typename T>
inline std::vector<std::vector<std::vector<T>>>
  Rdin(
    const char* name,
    const char* strate1, Levels levels1,
    const char* strate2, Levels levels2,
    const char* strate3, Levels levels3) {
  auto c = Context(Context::env,name,true);
  Rcpp::IntegerVector factor1 = Rinput(strate1,levels1);
  Rcpp::IntegerVector factor2 = Rinput(strate2,levels2);
  Rcpp::IntegerVector factor3 = Rinput(strate3,levels3);
  if(factor1.size() != levels1.size() * levels2.size() * levels3.size()) {
    Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' comporte des doublons" << std::endl;
    ::Rf_error("parametre incorrect");
  }

  std::vector<std::vector<std::vector<T>>> tmp;
  int i = 0;
  for(auto l1 : levels1.get_levels()) {
    tmp.emplace_back();
    for(auto l2 : levels2.get_levels()) {
      tmp.back().emplace_back();
      for(auto l3 : levels3.get_levels()) {
        if(factor1[i]!=l1+1 || factor2[i]!=l2+1 || factor3[i]!=l3+1) {
          Rcpp::Rcerr << "L'objet '" << strate1 << "' de la liste '" <<  name  << "' n'est pas trié correctement." << std::endl;
          ::Rf_error("parametre incorrect");
        }
        tmp.back().back().emplace_back();
        c.Next();
        i++;
      }
    }
  }
  return tmp;
}

#define _(name,...) name = Rinput(#name,##__VA_ARGS__)
#define levels(X...) Levels({X}, #X)

/*
template<typename T>
std::vector<T> Rdin(Rcpp::Environment& env, const char* name, Levels un) {
  std::vector<T> v;
  int lenght = un.m_labels.size();
  v.reserve(lenght);
  for(int i = 0; i < lenght; i++)
      v.emplace_back(Rdin<T>(env,name));
  return v;
}

template<typename T>
vector<T> importdf(Rcpp::Environment& env, int lenght) {
  vector<T> v;
  v.reserve(lenght);
  for(int i = 0; i < lenght; i++)
      v.emplace_back(env,i);
  return v;
}

template<typename T>
vector<vector<T>> importdf(Rcpp::Environment& env, int lenght, int dim2) {
  vector<vector<T>> v;
  v.reserve(lenght);
  for(int i = 0; i < lenght; i++) {
      v[i].reserve(dim2);
      for(int j = 0; j < dim2; j++)
        v[i].emplace_back(env,i);
  }

  return v;
}

template<typename T>
vector<vector<vector<T>>> importdf(Rcpp::Environment& env, int lenght, int dim2, int dim3) {
  vector<vector<vector<T>>>  v;
  //v.reserve(lenght);
  for(int i = 0; i < lenght; i++) {
    v.emplace_back();
    for(int j = 0; j < dim2; j++) {
      v[i].emplace_back();
      for(int k = 0; k < dim3; k++)
        v[i][j].emplace_back(env,i*(dim3*dim2)+j*dim3+k);
    }
  }

  return v;
}


template<typename T>
vector<vector<vector<vector<T>>>>
importdf(Rcpp::Environment& env, int lenght, int dim2, int dim3, int dim4) {
  vector<vector<vector<T>>>  v;
  v.reserve(lenght);
  for(int i = 0; i < lenght; i++) {
    v[i].reserve(dim2);
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
        v[k].emplace_back(env,k);
  }

  return v;
}
*/

class Rdout {
  template<typename T> using deque = std::deque<T>;
  template<typename T> using vector = std::vector<T>;
  using uint = unsigned int;
  using string = std::string;
  using llui = long long unsigned int;

public:

  Rdout(string&& name, deque<string>&& col_names) :
    m_nv(), m_iv(), m_lv(), m_cv(),
    m_col_names(col_names), m_df_name(name),  m_map_labels()
  {
    Context::add_df(this);
  }

  template<typename... Args>
  Rdout(string&& name, deque<string>&& col_names, Args&&... args) :
    m_nv(), m_iv(), m_lv(), m_cv(),
    m_col_names(col_names), m_df_name(name),  m_map_labels()
  {
    Context::add_df(this);
    flabels(std::forward<Args>(args)...);
  }


  template<typename...Args>
	void push_line(Args&&...args) {	pushl<0>(std::forward<Args>(args)...); }

private:
	//template<int N, class...Args> void pushl(Rcpp::internal::string_proxy<16>&& val, Args&...args)  { m_cv[N].push_back(Rcpp::as<string>(val)); std::cout << "stri"<< std::endl; pushl<N+1>(std::forward<Args>(args)...); }
	//template<int N, class...Args> void pushl(Rcpp::internal::string_proxy<16>& val, Args&...args)  { m_cv[N].push_back(Rcpp::as<string>(val)); std::cout << "stri"<< std::endl; pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(double&& val, Args&&...args)  { m_nv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(const double& val, Args&&...args)  { m_nv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(int&& val, Args&&...args)    { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(const int& val, Args&&...args)     { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(uint&& val, Args&&...args)    { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(const long unsigned int& val, Args&&...args)     { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(long unsigned int&& val, Args&&...args)    { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(const uint& val, Args&&...args)    { m_iv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(bool&& val, Args&&...args)    { m_lv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(const bool& val, Args&&...args)    { m_lv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(llui&& val, Args&&...args)    { m_nv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(const llui& val, Args&&...args)    { m_nv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...);}
	template<int N, class...Args> void pushl(string&& val, Args&&...args)  { m_cv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }
	template<int N, class...Args> void pushl(const string& val, Args&&...args)  { m_cv[N].push_back(val); pushl<N+1>(std::forward<Args>(args)...); }

  template<int N>	void pushl() {}
  /*template<int N, typename...Args>
  void pushl(Indicateur& val, Args&...args) {
  	m_nv[N].push_back(val.resultat());
		pushl<N+1>(args...);
  }*/
  
/*
  template<typename... Args>
  void flabels(const char* test, Args&&... args) {Rcpp::Rcout << test << std::endl;}
*/
  template<typename... Args>
  void flabels(string&& str, Levels&& levels, Args&&... args)  {
    m_map_labels[str] = levels.get_labels();
    flabels(std::forward<Args>(args)...);
  }

  template<typename... Args>
  void flabels(const string& str, const Levels& levels, Args&&... args)  {
    m_map_labels[str] = levels.get_labels();
    flabels(std::forward<Args>(args)...);
  }
  /*
  template<typename... Args>
  void flabels(string str, const Levels& levels, Args&&... args)  {
    m_map_labels[str.c_str()] = levels.get_labels();
    flabels(std::forward<Args>(args)...);
  }*/
  /*
  template<int N, typename... Args>
  void flabels(const char str[N], const Levels& levels, Args&&... args)  {
    m_map_labels[str] = levels.get_labels();
    flabels(std::forward<Args>(args)...);
  }
  
  template<int N, typename... Args>
  void flabels(const char str[N], Levels&& levels, Args&&... args)  {
    const char* str2 = str;
    m_map_labels[str2] = levels.get_labels();
    flabels(std::forward<Args>(args)...);
  }  */

  void flabels()  {}

public:
  void export_df(Rcpp::Environment& env) {
    if(m_nv[0].size() + m_iv[0].size() + m_lv[0].size() + m_cv[0].size() == 0) {
      return;
    }
    else {
      env[m_df_name.c_str()] = as_data_frame();
    }
  }

private:
	Rcpp::DataFrame as_data_frame() {
    Rcpp::List df;

    unsigned int size = 0;
		for (unsigned int i = 0; i < m_col_names.size(); i++) {
		  auto& col_name = m_col_names[i];
		  auto& dnv = m_nv[i];
		  auto& div = m_iv[i];
		  auto& dlv = m_lv[i];
		  auto& dcv = m_cv[i];

			if (dnv.size()) {
			  if(dnv.size() > size) size = dnv.size();
        df[col_name.c_str()] = Rcpp::wrap(dnv);
        dnv.clear();
			}
			if (div.size()) {
			  if(div.size() > size) size = div.size();
        Rcpp::IntegerVector iv = Rcpp::wrap(div);
        div.clear();
        if(m_map_labels[col_name].size() > 0) {
          iv = iv + 1;
          iv.attr("levels") = Rcpp::wrap(m_map_labels[col_name]);
          iv.attr("class") = "factor";
        }
        df[col_name.c_str()] = iv;
			}
			if (dlv.size()) {
			  if(dlv.size() > size) size = dlv.size();
        df[col_name.c_str()] = Rcpp::wrap(dlv);
        dlv.clear();
			}
			if (dcv.size()) {
			  if(dcv.size() > size) size = dcv.size();
        df[col_name.c_str()] = Rcpp::wrap(dcv);
        dcv.clear();
			}
		}

		
		Rcpp::Rcout << "export : " << m_df_name << " ("<< size << " lines)" << std::endl;
    Rcpp::IntegerVector row_names(size);
    for (unsigned int i = 0; i < size; ++i) {
       row_names(i) = i;
    }
    
    df.attr("row.names") = row_names;
    Rcpp::CharacterVector classes = {"data.frame","tbl_df","tbl"};
    df.attr("class") = classes;
    return df;
	}


private:
  deque<double> m_nv[128];
  deque<int> m_iv[128];
  deque<bool> m_lv[128];
  deque<string> m_cv[128];

  deque<string> m_col_names;
  string m_df_name;
  Rcpp::Environment m_df_env;
	std::map<string, vector<string>> m_map_labels;
};

inline void export_df(Rcpp::Environment& env, Rdout& df) {
  df.export_df(env);
}


typedef std::deque<std::string> vars;



template<int size, char... c>
struct CString {
  static inline constexpr const std::array<char,size+1> get() {
    return std::array<char,size+1>({c...,'\0'});
  }
};


// Destinie 2
// Copyright © 2005-2018, Institut national de la statistique et des études économiques
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
