#include "main.h"

// to test KR, JP, CN, GR, IS, FINikieciu, ARAB

int main(){
    std::map<std::string, int> zodziai;
    bool egzistuoja;
    std::string failoPavadinimas;
    failoPasirinkimas(egzistuoja, failoPavadinimas);
    if(!egzistuoja){
        return 0;
    }
    std::ifstream in(failoPavadinimas);
    std::string zodis;
    std::cout << zodis << "\n";
    while(in >> zodis)
    {
        zodis = cleanZodis(zodis);
        if(!zodis.empty())
        {
            ++zodziai[zodis];
        }
    }
    int sum = 0;
    std::ofstream out("zodziai.txt");
    for(auto X : zodziai)
    {
        if(X.second>1)
            std::cout << X.first << " " << X.second << std::endl;

        sum+=X.second;
    }
    std::cout << sum << std::endl;
    return 0;
}