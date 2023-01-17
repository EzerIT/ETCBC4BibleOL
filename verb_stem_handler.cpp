/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <string>

#include "verb_stem_handler.hpp"

using namespace std;

class verb_stem_handler : public handler {
  public:
    virtual string pre_create() override;
    virtual string handler_name() const override;
};

shared_ptr<handler> make_verb_stem_handler()
{
    return shared_ptr<handler>{new verb_stem_handler};
}

string verb_stem_handler::pre_create()
{
    return
        "UPDATE ENUMERATION verbal_stem_t = {\n"
        "    REMOVE esht,\n"
        "    REMOVE etta\n"
        "} GO\n";
}

string verb_stem_handler::handler_name() const
{
    return "verb_stem_handler";
}


