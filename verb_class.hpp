/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#ifndef VERB_CLASS_HPP
#define VERB_CLASS_HPP

#include <string>
#include <set>

void build_newgloss(bool is_hebrew);

std::set<std::string>& verbclasses_lookup(bool is_hebrew, const std::string &lex);

#endif // VERB_CLASS_HPP

