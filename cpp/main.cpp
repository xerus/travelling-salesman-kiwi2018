#include <random>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>


typedef unsigned Airport;
typedef unsigned Area;
typedef std::unordered_set<Airport> UniqueAirports;
typedef std::unordered_set<Area> UniqueAreas;
typedef std::unordered_map<Area, UniqueAirports> strDict;
typedef std::unordered_map<Airport, unsigned> toPrice;
typedef std::vector<Airport> Way;
static Airport start;
static strDict airports;
static std::unordered_map<Airport, Area> areas;
static unsigned N;
// day -> from -> to -> price
static std::vector<std::unordered_map<Airport, toPrice>> timetable;
static auto maxTime = std::chrono::high_resolution_clock::now();
static auto currentTime = std::chrono::high_resolution_clock::now();
static unsigned bestPrice = 0;
static Way bestWay;

static std::unordered_map<std::string, Airport> strToAirportId;
static std::unordered_map<Airport, std::string> airportIdtoStr;

static std::mt19937 g(10);

#define STDIN std::cin

static void parseInput() {
    std::string startStr;
    unsigned airportId = 0;
    std::ios_base::sync_with_stdio(false);
    std::string line;
    std::getline(STDIN, line);
    std::stringstream ss(line);
    ss >> N >> startStr;
    N = N + 1;
    timetable.resize(N);
    for (unsigned i = 0; i < N - 1; i++) {
        std::getline(STDIN, line); // skipline
        const unsigned area = i;
        std::string port;
        airports[area] = UniqueAirports();
        std::getline(STDIN, line);
        std::stringstream ss(line);
        while (std::getline(ss, port, ' ')) {
            strToAirportId[port] = airportId;
            airportIdtoStr[airportId] = port;
            airports[area].insert(airportId);
            areas[airportId] = area;
            airportId++;
        }
    }
    std::string ff, tt;
    Airport f, t;
    unsigned d, c;
    start = strToAirportId[startStr];
    while (STDIN >> ff >> tt >> d >> c) {
        f = strToAirportId[ff];
        t = strToAirportId[tt];
        if (areas[f] == areas[t])
            continue;
        unsigned startDay = d, endDay = d + 1;
        if (d == 0) {
            if (f == start)
                startDay = 1;
            else if (areas[t] == areas[start])
                startDay = N - 1;
            else
                startDay = 2;
            endDay = N;
        }
        for (unsigned day = startDay; day < endDay; day++) {
            if (day == N - 1 && areas[t] != areas[start])
                continue;
            auto el = timetable[day].find(f);
            if (el == timetable[day].end()) {
                timetable[day][f] = {{t, c}};
            } else {
                auto elT = el->second.find(t);
                if (elT == el->second.end()) {
                    el->second[t] = c;
                } else if (elT->second > c) {
                    elT->second = c;
                }
            }
        }
    }
}


static bool possibleAirports(const unsigned day, const Airport &from,
                             const UniqueAreas &visited,
                             std::vector<Airport> &dests,
                             const bool greedy, const bool random) {
    const auto el = timetable[day].find(from);
    if (el == timetable[day].end())
        return false;
    for (const auto &tc : el->second) {
        if (day < N - 1 && visited.find(areas[tc.first]) != visited.end())
            continue;
        dests.push_back(tc.first);
    }
    if (random && day < N - 1) {
        std::shuffle(dests.begin(), dests.end(), g);
    } else if (greedy || day >= N - 1) {
        std::sort(dests.begin(), dests.end(),
                  [&](const Airport &a, const Airport &b) {
            return el->second[a] < el->second[b];
        });
    }
    return !dests.empty();
}

static unsigned findWay(const Airport &ns, UniqueAreas &visited,
                        Way &way, const bool greedy, const bool random,
                        int tries,
                        const unsigned day, const unsigned price) {
    unsigned currentPrice = 0;
    currentTime = std::chrono::high_resolution_clock::now();
    if (currentTime >= maxTime) {
        return 0;
    }
    if (bestPrice != 0 && bestPrice < price) {
        return price;
    }
    std::vector<Airport> dests;
    dests.reserve(16);
    if (day == N - 1) {
        if (!possibleAirports(day, ns, visited, dests, greedy, random))
            return 0;
        Airport bestEnd = dests.front();
        way[day - 1] = bestEnd;
        currentPrice = price + timetable[day][ns][bestEnd];
        if (bestPrice == 0 || currentPrice < bestPrice) {
            bestWay = way;
            bestPrice = currentPrice;
        }
        return currentPrice;
    }
    UniqueAreas newVisited(visited);
    newVisited.emplace(areas[ns]);
    if (!possibleAirports(day, ns, newVisited, dests, greedy, random))
        return 0;

    Airport bestDest = way[day - 1];
    for (const auto &p : dests) {
        if (currentTime >= maxTime) {
            break;
        }
        way[day - 1] = p;
        unsigned newPrice = price + timetable[day][ns][p];
        newPrice = findWay(p, newVisited, way, greedy, random,
                           tries, day + 1, newPrice);
        if (newPrice == 0) {
            continue;
        }
        if (currentPrice == 0 || newPrice < currentPrice) {
            currentPrice = newPrice;
            bestDest = p;
        }
        if (--tries <= 0) {
            break;
        }
    }
    way[day - 1] = bestDest;
    return currentPrice;
}


// static void printTimetable() {
//     for (unsigned day = 0; day < N; day++) {
//         std::cerr << "day: " << day << std::endl;
//         for (const auto &ft : timetable[day]) {
//             for (const auto &tc : ft.second) {
//                 std::cerr << airportIdtoStr[ft.first] << " -> "
//                           << airportIdtoStr[tc.first] << ": "
//                           << tc.second << std::endl;
//             }
//         }
//     }
// }


int main() {
    parseInput();
#ifdef DEBUG_OUTPUT
    unsigned generated = 0;
    currentTime = std::chrono::high_resolution_clock::now();
    auto diff = currentTime - maxTime;
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    std::cerr << " Areas: " << N - 1 << std::endl
              << " Seconds to parse input: "
              << millis.count() / 1000.
              << std::endl;
#endif
    int tries;
    if (N <= 21) {
        maxTime += std::chrono::milliseconds(2500);
        tries = 6;
    } else if (N <= 101) {
        maxTime += std::chrono::milliseconds(4500);
        tries = 4;
    } else {
        maxTime += std::chrono::milliseconds(14200);
        tries = 4;
    }
    // printTimetable();
    UniqueAreas visited;
    Way way(N - 1);
    bestWay.resize(N - 1);
    findWay(start, visited, way, true, false, tries, 1, 0);
    while (currentTime < maxTime) {
        findWay(start, visited, way, false, true, tries, 1, 0);
        tries++;
#ifdef DEBUG_OUTPUT
        generated++;
#endif
    }
#ifdef DEBUG_OUTPUT
    std::cerr << " Randomly generated: " << generated << std::endl;
#endif
    if (!bestPrice) {
        std::cerr << "way not found" << std::endl;
        return 1;
    }
    std::cout << bestPrice << std::endl;
    Airport ns = start;
    for (unsigned day = 1; day < N; day++) {
        const Airport t = bestWay[day - 1];
        std::cout << airportIdtoStr[ns] << " " << airportIdtoStr[t] << " "
                  << day << " "
                  << timetable[day][ns][t] << std::endl;
        ns = t;
    }
    return 0;
}
