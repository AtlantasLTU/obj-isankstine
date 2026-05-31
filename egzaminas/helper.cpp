#include "helper.h"

int menu()
{
    return gautiSkaiciu("Pasirinkite programos eigą:\n1 - skaityti tekstą iš failo,\n2 - generuoti tik pažymius,\n3 - generuoti studentų vardus, pavardės ir pažymius,\n4 - skaityti studentus iš failo,\n5 - testavimas su failais,\n6 - generuoti failą,\n7 - testuoti klasę Studentas,\n8 - nuosavo vektoriaus ir STL vektoriaus palyginimas,\n9 - baigti darbą: ", 1, 9);
}

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
    std::vector<int> rezervai;

    for(auto &failas : std::filesystem::directory_iterator(vieta))
    {
        if(!failas.is_regular_file())
            continue;

        if(failas.path().extension() != ".txt")
            continue;

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
        "„", "“", """, """, 
        "«", "»", "‹", "›", 
        "–", "—", "•", "−", 
        "′", "’", "■", "\xE2\x80\x8B",
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
        for (const auto& q : quotes) {
            if (zodis.compare(i, q.size(), q) == 0) {
                isQuote = true;
                i += q.size();
                break;
            }
        }
        if (isQuote) continue;
        if (c <= 0x7F) {
            if (!std::ispunct(c) && !std::isdigit(c))
                clean += static_cast<char>(c);
            i++;
        } else {
            size_t charLen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : 2;
            UChar32 cp;
            int32_t offset = 0;
            U8_NEXT(zodis.data() + i, offset, (int32_t)charLen, cp);

            if (u_isalpha(cp) || u_getCombiningClass(cp) > 0)
                clean.append(zodis, i, charLen);

            i += charLen;
        }
    }

    return toLowerUnicode(clean);
}


std::string toLowerUnicode(const std::string& str) {
    icu::UnicodeString us = icu::UnicodeString::fromUTF8(str);
    us.toLower();
    std::string result;
    us.toUTF8String(result);
    return result;
}

void crossReference(const std::string& fileName) {
    std::ifstream in(fileName);
    std::map<std::string, std::set<int>> wordToLines;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        ++lineNum;
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            word = cleanZodis(word);
            if (!word.empty()) {
                wordToLines[word].insert(lineNum);
            }
        }
    }

    std::ofstream out("cross_ref.txt");
    for (const auto& [word, lines] : wordToLines) {
        if (lines.size()>1) {
            out << word << ": ";
            for (auto it = lines.begin(); it != lines.end(); ++it) {
                if (it != lines.begin()) out << ", ";
                out << *it;
            }
            out << "\n";
        }
    }
}

void extractUrls(const std::string& fileName) {
    std::ifstream in(fileName);
    std::string content((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    std::regex url_regex(R"((https?://)?(www\.)?([a-zA-Z0-9][a-zA-Z0-9.-]*\.[a-zA-Z]{2,})(/[^\s]*)?)");
    std::set<std::string> found;
    std::sregex_iterator it(content.begin(), content.end(), url_regex);
    std::sregex_iterator end;
    for (; it != end; ++it) {
        std::string domain = (*it)[3];
        std::unordered_set<std::string> tlds = loadTlds("tlds-alpha-by-domain.txt");
        size_t last_dot = domain.rfind('.');
        if (last_dot != std::string::npos) {
            std::string tld = domain.substr(last_dot + 1);
            if (tlds.find(tld) != tlds.end()) {
                found.insert(it->str());
            }
        }
    }
    std::ofstream out("urls.txt");
    for (const auto& url : found) {
        out << url << "\n";
    }
}

std::unordered_set<std::string> loadTlds(const std::string& path) {
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