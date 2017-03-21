/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#ifndef VERB_CLASS_HPP
#define VERB_CLASS_HPP

#include <string>
#include <set>

void build_newgloss();

std::set<std::string>& verbclasses_lookup(const std::string &lex);

#endif // VERB_CLASS_HPP

