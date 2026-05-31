#ifndef HELPER_H
#define HELPER_H

#include <unicode/unistr.h>
#include <unicode/uchar.h>
#include <unicode/utf8.h>
#include <map>
#include <set>
#include <regex>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <sstream>

/**
 * @brief Nuskaito sveikąjį skaičių iš konsolės su patikrinimais.
 * @param pranešimas - tekstas, rodomas prieš įvedimą.
 * @param min Mažiausia leistina reikšmė.
 * @param max Didžiausia leistina reikšmė.
 * @param galiButiTuscia Ar galima įvesti tuščią eilutę (grąžina -1).
 * @return Įvestas skaičius.
 */
int gautiSkaiciu(const std::string &pranešimas, int min, int max, bool galiButiTuscia = false);

/**
 * @brief Patikrina, ar eilutę sudaro tik skaitmenys ir tarpai.
 * @param s Tikrinama eilutė.
 * @return true jei taip.
 */
bool arTikSkaicius(const std::string &s);

/**
 * @brief Apdoroja EOF atvejį cin sraute.
 */
void cinEOFgaudymas();

/**
 * @brief Leidžia vartotojui pasirinkti failą iš esamų .txt failų programos direktorijoje.
 * @param egzistuoja Ar rastas bent vienas tinkamas failas.
 * @param failoPavadinimas Pasirinkto failo pavadinimas.
 * @param vieta direktorija, kurioje numatyta ieškoti failų (numatyta ".").
 */
void failoPasirinkimas(bool &egzistuoja, std::string &failoPavadinimas, const std::string& vieta = ".");

std::string cleanZodis(const std::string& zodis);

std::string toLowerUnicode(const std::string& zodis);

void crossReference(std::stringstream& in);

void extractUrls(std::stringstream& in);

std::unordered_set<std::string> loadTlds(const std::string& path);

std::vector<std::string> splitAndClean(const std::string& zodis);

void extractZodziai(std::stringstream& in);

#endif