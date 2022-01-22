/* Copyright © 2017 Ezer IT Consulting.
 * Released under an MIT License.
 */
 
#include <string>
#include <cassert>

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
    req.insert({"prs","g_prs","language","suffix_gender","prs_gn"});
}


string suffix_handler::pre_create()
{
    return
        "UPDATE ENUMERATION person_t = {\n"
        "    ADD absent = 4\n"
        "} GO\n"
        "UPDATE ENUMERATION number_t = {\n"
        "    ADD absent = 4\n"
        "} GO\n"
        "UPDATE ENUMERATION gender_t = {\n"
        "    ADD absent = 3\n"
        "} GO\n";
}

string suffix_handler::update_object(const map<string,string>& fmap)
{
    if (fmap.at("prs")=="absent")
        return
            "    suffix_gender := absent;\n" 
            "    suffix_number := absent;\n" 
            "    suffix_person := absent;\n";

    else if (fmap.at("language")=="Aramaic") { // Fix errors in gender suffix

        if (fmap.at("g_prs")=="+AH.") {
            assert(fmap.at("suffix_gender")=="m");
            assert(fmap.at("prs_gn")=="m");
            return
                "    suffix_gender := f;\n"
                "    prs_gn := f;\n";
        }

        else if (fmap.at("g_prs")=="+;H.") {
            assert(fmap.at("suffix_gender")=="f");
            assert(fmap.at("prs_gn")=="f");
            return
                "    suffix_gender := m;\n"
                "    prs_gn := m;\n";
        }
    }
    return "";
}
