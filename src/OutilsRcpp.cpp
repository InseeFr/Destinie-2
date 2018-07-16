#include "OutilsRcpp.h"

Rcpp::Environment Context::env = Rcpp::Environment();
Rcpp::List Context::lst = Rcpp::List();
const char* Context::lst_name = "";
int Context::iter = 0;
int Context::check = Context::NOCHECK;
std::deque<Rdout*> Context::list_df;


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