/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <string>

#include "suffix_handler.hpp"

using namespace std;

class suffix_handler : public handler {
  public:
    virtual void list_features(set<string>&) override;
    virtual string pre_create() override;
    virtual string update_object(const map<string,string>&) override;
};


shared_ptr<handler> make_suffix_handler()
{
    return shared_ptr<handler>{new suffix_handler};
}

void suffix_handler::list_features(set<string>& req)
{
    req.insert({"prs"});
}


string suffix_handler::pre_create()
{
    return
        "UPDATE ENUMERATION person_t = {\n"
        "    ADD absent = 4,\n"
        "}\n"
        "UPDATE ENUMERATION number_t = {\n"
        "    ADD absent = 4,\n"
        "}\n"
        "UPDATE ENUMERATION gender_t = {\n"
        "    ADD absent = 3,\n"
        "}\n"
        "GO\n";
}

string suffix_handler::update_object(const map<string,string>& fmap)
{
    if (fmap.at("prs")=="absent")
        return
            "    suffix_gender := absent;\n" 
            "    suffix_number := absent;\n" 
            "    suffix_person := absent;\n";
    else
        return "";
}
