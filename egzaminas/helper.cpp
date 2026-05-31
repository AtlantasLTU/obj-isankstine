#include "helper.h"

int gautiSkaiciu(const std::string &pranešimas, int min, int max, bool galiButiTuscia /* = false */)
{
    std::string ivestis;
    while (true) {
        std::cout << pranešimas;

        if (!std::getline(std::cin, ivestis)) {
            if (std::cin.eof()) 
                cinEOFgaudymas();
            continue;
        }

        // ivesties nutraukimas su ENTER
        if (galiButiTuscia && ivestis.empty()) return -1;

        try {
            // 
            if (!arTikSkaicius(ivestis)) throw std::invalid_argument("Ne skaičius");

            int skaicius = std::stoi(ivestis);

            //tikriname ar ivestas skaicius atitinka nuo maziausio leistino iki didziausio leistino
            if (skaicius >= min && skaicius <= max) {
                return skaicius;
            } else {
                std::cout << "Klaida, skaičius turi būti tarp " << min << " ir " << max << "!\n";
            }
        } catch (...) {
            std::cout << "Klaida, įveskite sveikąjį skaičių!\n";
        }
    }
}

bool arTikSkaicius(const std::string& eilute)
{ // jei eilute tuscia grazinama false, std::all_of pereina nuo eilutes.begin() pradzios iki galo eilutes.end() per kiekviena simboli, kiekvienam simboliui jei jis skaicius ar tarpas grazina true, jei tai tiesiog raide - grazinama false ir toliau eilute nebetikrinama
    return !eilute.empty() && 
    std::all_of(eilute.begin(), eilute.end(), [](unsigned char simbolis) 
    {
        return std::isdigit(simbolis) || std::isspace(simbolis);
    });
}

void cinEOFgaudymas()
{
    if (std::cin.eof())
    { // apsauga nuo CTRL+D (linux), CTRL+Z (windows)
        throw std::runtime_error("Įvesties pabaiga (EOF). Darbas su programa baigtas");
    }
    std::cin.clear(); // atstatome std::cin fail flag'a
}

void failoPasirinkimas(bool &egzistuoja, std::string &failoPavadinimas, const std::string& vieta)
{
    std::vector<std::filesystem::directory_entry> failai;

    for(auto &failas : std::filesystem::directory_iterator(vieta))
    {
        if(!failas.is_regular_file())
            continue;

        if(failas.path().extension() != ".txt")
            continue;

        std::string vardas = failas.path().filename().string();

        if(vardas == "urls.txt" ||
            vardas == "cross_ref.txt" ||
            vardas == "zodziai.txt" ||
            vardas == "tlds-alpha-by-domain.txt"
        ) {
            continue;
        }

        failai.push_back(failas);
    }

    if(failai.empty())
    {
        std::cout << "Nerasta tekstinių failų vietoje: " << vieta << "\n";
        egzistuoja = false;
    } 
    else 
    {
        std::cout << "Pasirinkite failą:\n";
        for(int i = 0; i < failai.size(); i++)
        {
            std::cout << (i + 1) << ": " << failai.at(i).path().filename().string() << "\n";
        }

        int pasirinkimas = gautiSkaiciu("Įveskite failo numerį: ", 1, failai.size(), false);
        failoPavadinimas = failai.at(pasirinkimas-1).path().filename().string();
        egzistuoja = true;
    }
}

std::string cleanZodis(const std::string& zodis) {
    std::string clean;
    const std::string quotes[] = { 
        "„", "“", "\"", "«",
        "»", "‹", "›", "–",
        "—", "•", "−", "′",
        "’", "■", "\xE2\x80\x8B",
        "\xE2\x80\x8C",
        "\xE2\x80\x8D",
        "\xE2\x80\x8E",
        "\xE2\x80\x8F",
        "\xE2\x80\xAB",
        "\xE2\x80\xAC",
        "\xEF\xBB\xBF",
    };
    
    size_t i = 0;
    while (i < zodis.size()) {
        unsigned char c = zodis[i];

        bool isQuote = false;
        for (const auto& q : quotes) { // pereiname pro quotes masyva, ieskodami ar zodzio simbolis sutampa su kazkuriuo is quotes (kabuciu) elementu
            if (zodis.compare(i, q.size(), q) == 0) {
                isQuote = true;
                i += q.size(); 
                break;
            }
        } // jeigu randamos kabutes, tai simbolis praleidziamas
        if (isQuote) continue;
        if (c <= 0x7F) { // jeigu simbolis ASCII, patikrina ar tai ne skyrybos zenklas, skaicius ar tarpas ir prideda ji prie clean string'o, taip kuria nauja svaru zodi
            if (!std::ispunct(c) && !std::isdigit(c) && !isspace(c))
                clean += static_cast<char>(c);
            i++;
        } else { // jeigu simbolis ne ASCII, tai 
            UChar32 cp;
            int32_t offset = 0;
            U8_NEXT(zodis.data() + i, offset, (int32_t)(zodis.size() - i), cp); // pavercia simboli esanti zodis.data()+i i unicode 32 bitu simboli

            if (cp >= 0 && (u_isalpha(cp) || u_getCombiningClass(cp) > 0)) // ziuri ar simbolio pavertimas pavyko ir ar jis abeceles simbolis arba kito simbolio dalis
                clean.append(zodis, i, offset); // i svaru zodi prideda simboli (siuo atveju offset kiekio baitu nuo i)

            i += offset; // prie i kaip iteratoriaus pridedamas offset, kad
        }
    }

    return toLowerUnicode(clean); // paima sukurta svaru zodi ir ji padaro lowercase, kad nebutu zodziu su didziosiomis ir mazosiomis, kaip skirtingu raktu
}


std::string toLowerUnicode(const std::string& str) {
    icu::UnicodeString us = icu::UnicodeString::fromUTF8(str); // pavercia UTF8 string'a i unicode character string klase
    us.toLower(); // naudoja tos klases metodus ir kiekviena simboli pavercia mazuoju
    std::string result;
    us.toUTF8String(result); // atgal pavercia i utf-8 string'a
    return result;
}

void crossReference(std::stringstream& in) {
    std::map<std::string, std::set<int>> wordToLines;
    std::map<std::string, int> wordCounts;
    
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) { // perskaito eilute
        ++lineNum;
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            for (const auto &w : splitAndClean(word)) { // apvalo eilutes zodi
                if (!w.empty()) {
                    wordToLines[w].insert(lineNum); // i wordToLines map'a ideda i rakta w eilutes skaiciu.
                    wordCounts[w]++; // i wordCounts map'a padidina rakto reiksme
                }
            }
        }
    }

    std::ofstream out("cross_ref.txt");
    for (const auto& [word, lines] : wordToLines) {
        // Filtruojame pagal bendrą pasikartojimų skaičių tekste
        if (wordCounts[word] > 1) { // jei rakto reiksme, t.y. zodis pasikartoja daugiau nei karta tai isveda ji
            out << word << ": ";
            for (auto it = lines.begin(); it != lines.end(); ++it) { // isveda eilutes, kuriose yra zodis
                if (it != lines.begin()) out << ", ";
                out << *it;
            }
            out << "\n";
        }
    }
}

void extractUrls(std::stringstream& in) { // faila iveda i content string'a
    std::string content((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    std::regex url_regex(R"((https?://)?(www\.)?([a-zA-Z0-9][a-zA-Z0-9.-]*\.[a-zA-Z]{2,})(/[^\s]*)?)"); // regex pagal kuri ieskome ar tai url
    std::set<std::string> found;
    std::sregex_iterator it(content.begin(), content.end(), url_regex);
    std::sregex_iterator end; // surandame url
    std::unordered_set<std::string> tlds = loadTlds("tlds-alpha-by-domain.txt"); // uzkrauname TLDS faila
    for (; it != end; ++it) {
        std::string domain = (*it)[3]; // istraukia domena
        size_t last_dot = domain.rfind('.'); // suranda paskutini taska
        if (last_dot != std::string::npos) { // jei rado taska, tai
            std::string tld = domain.substr(last_dot + 1); // paima domeno galune t.y. lt, gov ir t.t.
            if (tlds.find(tld) != tlds.end()) { // patikrina ar domenas egzistuoja tlds rinkiny
                found.insert(it->str()); // rasta domena prideda i rastu domenu set'a
            }
        }
    }
    std::ofstream out("urls.txt");
    for (const auto& url : found) { // isveda rastu domenu set'a i urls.txt faila
        out << url << "\n";
    }
}

std::unordered_set<std::string> loadTlds(const std::string& path) { // uzkrauna tlds faila mazosiomis
    std::ifstream in(path);
    std::unordered_set<std::string> tlds;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        tlds.insert(line);
    }
    return tlds;
}

std::vector<std::string> splitAndClean(const std::string& zodis) {
    const std::string separators[] = { 
        "–", "—", "−", "."
    };

    size_t splitPos = std::string::npos;
    size_t splitLen = 0;
    for (const auto& sep : separators) {
        size_t pos = zodis.find(sep); // bando surast skirties zenkla "-" ir t.t.
        if (pos != std::string::npos && (splitPos == std::string::npos || pos < splitPos)) { // patikrina ar rado skirties zenkla, ar nera nustatyta jo pozicija, arba jei yra tai gal naujai rasto zenklo pozicija mazesne, t.y. bandom rast pati kairini
            splitPos = pos;
            splitLen = sep.size();
        }
    }

    if (splitPos == std::string::npos) { // jei nerado skirties zenklu, tai grazina svaru zodi
        std::string cleaned = cleanZodis(zodis);
        if (!cleaned.empty()) return { cleaned };
        return {};
    }

    std::vector<std::string> result;
    for (const std::string& part : { // buvusio zodzio dalis rekursijos budu toliau skaido, galu gale apvalo ir grazina kaip rezultata.
        zodis.substr(0, splitPos),
        zodis.substr(splitPos + splitLen)
    }) {
        auto sub = splitAndClean(part);
        result.insert(result.end(), sub.begin(), sub.end());
    }
    return result;
}

void extractZodziai(std::stringstream& in) // isvedam zodzius ir ju pasikartojimu kieki.
{
    std::map<std::string, int> zodziai;
    std::string zodis;
    while (in >> zodis)
    {
        for (const auto &w : splitAndClean(zodis))
        {
            if (!w.empty())
                ++zodziai[w];
        }
    }
    std::ofstream out("zodziai.txt");
    for (auto X : zodziai)
    {
        if (X.second > 1)
            out << X.first << " " << X.second << "\n";
    }
}