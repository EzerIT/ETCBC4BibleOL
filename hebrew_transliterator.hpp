/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#ifndef _HEBREW_TRANSLITERATOR_HPP
#define _HEBREW_TRANSLITERATOR_HPP

#include <string>
#include <map>

void initialize_translit_rules();
void initialize_translit_verbrules();

std::string transliterate(std::string input, std::string preceding_text, std::string following_text,
                          const std::map<std::string, std::string>& input_attributes, bool uc, bool do_qere_perpetuum, bool cantil_available);

std::string transliterate_verb_lex(const std::string& input);

std::string suffix_transliterate(const std::string& heb_suffix);


#endif // _HEBREW_TRANSLITERATOR_HPP
