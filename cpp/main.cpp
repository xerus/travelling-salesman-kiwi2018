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
#include <limits>


const static unsigned maxVal = std::numeric_limits<unsigned>::max();
typedef unsigned Airport;
typedef unsigned Area;
typedef std::unordered_set<Airport> UniqueAirports;
// Area -> visited?
typedef std::vector<bool> UniqueAreas;
// Area -> set(Airports)
typedef std::vector<UniqueAirports> strDict;
// Airport -> Price
typedef std::vector<unsigned> toPrice;
typedef std::vector<Airport> Way;
static Airport start;
static strDict airports;
// Airport -> Area
static std::vector<Area> areas;
static unsigned N;
// day -> from -> to -> price
static std::vector<std::vector<toPrice>> timetable;
// day -> from -> set(to)
static std::vector<std::vector<std::vector<Airport>>> prev, next;
static auto maxTime = std::chrono::high_resolution_clock::now();
static auto currentTime = std::chrono::high_resolution_clock::now();
static unsigned bestPrice = maxVal;
static Way bestWay;

static std::unordered_map<std::string, Airport> strToAirportId;
// Airport -> string
static std::vector<std::string> airportIdtoStr;

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
    areas.reserve(2 * N);
    airportIdtoStr.reserve(2 * N);
    airports.resize(N - 1,  UniqueAirports());
    for (unsigned area = 0; area < N - 1; area++) {
        std::getline(STDIN, line); // skipline
        std::string port;
        std::getline(STDIN, line);
        std::stringstream ss(line);
        while (std::getline(ss, port, ' ')) {
            strToAirportId[port] = airportId;
            airportIdtoStr.push_back(port);
            airports[area].insert(airportId);
            areas.push_back(area);
            airportId++;
        }
    }

    next.resize(N, std::vector<std::vector<Airport>>(airportId,
                                                     std::vector<Airport>()));
    prev.resize(N, std::vector<std::vector<Airport>>(airportId,
                                                     std::vector<Airport>()));
    timetable.resize(N, std::vector<toPrice>(airportId,
                                             toPrice(airportId, maxVal)));

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
            next[day][f].push_back(t);
            const unsigned currentPrice = timetable[day][f][t];
            if (c < currentPrice) {
                timetable[day][f][t] = c;
            }
        }
    }
    for (unsigned day = 1; day < N; day++) {
        for (unsigned f = 0; f < airportId; f++) {
            std::sort(next[day][f].begin(), next[day][f].end(),
                [&](const Airport &a, const Airport &b) {
                    return timetable[day][f][a] < timetable[day][f][b];
            });
            next[day][f].erase(std::unique(next[day][f].begin(),
                                           next[day][f].end()),
                               next[day][f].end());
        }
    }
}


static bool possibleAirports(const unsigned day, const Airport &from,
                             const UniqueAreas &visited,
                             std::vector<Airport> &dests,
                             const bool greedy, const bool random) {
    for (const auto to : next[day][from]) {
        if (day < N - 1 && visited[areas[to]])
            continue;
        dests.push_back(to);
    }
    if (random && day < N - 1) {
        std::shuffle(dests.begin(), dests.end(), g);
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
    if (bestPrice < price) {
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
        if (currentPrice < bestPrice) {
            bestWay = way;
            bestPrice = currentPrice;
        }
        return currentPrice;
    }
    UniqueAreas newVisited(visited);
    newVisited[areas[ns]] = true;
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

static unsigned wayCost(const Way &way,
                        const unsigned startDay = 1, /* 1 and greater */
                        const unsigned endDay = N /* N and smaller */) {
    unsigned cost = 0;
    Airport from = (startDay > 1) ? way[startDay - 2] : start;
    for (unsigned day = startDay; day < endDay; day++) {
        Airport to = way[day - 1];
        cost += timetable[day][from][to];
        from = to;
    }
    return cost;
}

// static bool reachableAirports(const unsigned day, const Airport &from,)


// static bool wayThru(const Way &way, const unsigned day,
//                     const Airport &from, const Airport &to,
//                     const Area &thru, Airport &transit) {
//     ;
// }


// static bool swapAreas(Way &way, const unsigned dayA, const unsigned dayB) {
//     ;
// }
// static bool optimizeWay(Way &way,
//                         const unsigned startDay,
//                         const unsigned endDay,
//                         int tries) {
//     ;
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
    UniqueAreas visited(N - 1, false);
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
    std::cout << bestPrice << std::endl;
#ifdef DEBUG_OUTPUT
    std::cerr << wayCost(bestWay) << std::endl;
#endif
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
