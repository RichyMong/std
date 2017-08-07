#include <iostream>
#include "pugixml.hpp"

using namespace std;

int main()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("pugi_test.xml");
    if (!result) {
        cerr << "Failed to load: " << result.description() << endl;
        return 1;
    }

    auto redis = doc.child("config").child("redis");
    cout << redis.attribute("sentinel") << endl;
    pugi::xpath_node_set tools = redis.select_nodes("//endpoint");
    for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it) {
        pugi::xpath_node node = *it;
        std::cout << node.node().attribute("ip").value() << "\n";
        std::cout << node.node().attribute("port").value() << "\n";
    }
    return 0;
}
