#include "main.h"

int main(){
    bool egzistuoja;
    std::string failoPavadinimas;
    failoPasirinkimas(egzistuoja, failoPavadinimas);
    if(!egzistuoja){
        return 0;
    }
    std::ifstream in(failoPavadinimas);
    std::stringstream buffer;
    buffer << in.rdbuf();
    extractZodziai(buffer);
    buffer.clear();
    buffer.seekg(0);
    crossReference(buffer);
    buffer.clear();
    buffer.seekg(0);
    extractUrls(buffer);
    return 0;
}