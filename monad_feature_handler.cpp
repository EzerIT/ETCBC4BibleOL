/* Copyright © 2022 Claus Tøndering
 * Released under an MIT License.
 */
 
#include <string>
#include <cassert>

#include "monad_feature_handler.hpp"

using namespace std;

class monad_feature_handler : public handler {
  public:
    virtual string define_features() override;
    virtual string update_object(const map<string,string>&) override;
    virtual string handler_name() const override;
};


shared_ptr<handler> make_monad_feature_handler()
{
    return shared_ptr<handler>{new monad_feature_handler};
}


string monad_feature_handler::define_features()
{
    return
        "    ADD monad_num : integer DEFAULT 0;\n";
}


string monad_feature_handler::update_object(const map<string,string>& fmap)
{
    return "    monad_num := " + fmap.at("monad") + ";\n";
}

string monad_feature_handler::handler_name() const
{
    return "monad_feature_handler";
}

    
