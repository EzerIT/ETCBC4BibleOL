/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#ifndef _UTIL_H
#define _UTIL_H

#include <string>

void replace_string_in_place(std::string& subject, const std::string& search, const std::string& replace);
std::string trim_spaces(const std::string& s);

bool ends_with(const std::string& full_string, const std::string& ending);

std::string fix_final(std::string s);

#endif // _UTIL_H
