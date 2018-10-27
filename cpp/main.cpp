#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>


typedef std::string Airport;
typedef std::string Area;
typedef std::unordered_set<Airport> UniquePlaces;
typedef std::unordered_map<Area, UniquePlaces> strDict;
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


static void parseInput() {
    std::string line, area;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    ss >> N >> start;
    N = N + 1;
    timetable.resize(N);
    for (unsigned i = 0; i < N - 1; i++) {
        std::getline(std::cin, area);
        Airport port;
        airports[area] = UniquePlaces();
        std::getline(std::cin, line);
        std::stringstream ss(line);
        while (std::getline(ss, port, ' ')) {
            airports[area].insert(port);
            areas[port] = area;
        }
    }
    Airport f, t;
    unsigned d, c;
    while (std::cin >> f >> t >> d >> c) {
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
                             const UniquePlaces &visited,
                             std::vector<Airport> &dests,
                             const bool greedy = true) {
    const auto el = timetable[day].find(from);
    if (el == timetable[day].end())
        return false;
    for (const auto &tc : el->second) {
        if (day < N - 1 && visited.find(areas[tc.first]) != visited.end())
            continue;
        dests.push_back(tc.first);
    }
    if (greedy) {
        std::sort(dests.begin(), dests.end(),
                  [&](const Airport &a, const Airport &b) {
            return el->second[a] < el->second[b];
        });
    }
    return !dests.empty();
}

static unsigned findWay(const Airport &ns, UniquePlaces &visited,
                        Way &way, const bool greedy,
                        const unsigned day, const unsigned price) {
    unsigned currentPrice = 0;
    currentTime = std::chrono::high_resolution_clock::now();
    if (currentTime >= maxTime) {
        return 0;
    }
    std::vector<Airport> dests;
    dests.reserve(16);
    if (day == N - 1) {
        if (!possibleAirports(day, ns, visited, dests))
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
    UniquePlaces newVisited(visited);
    newVisited.emplace(areas[ns]);
    if (!possibleAirports(day, ns, newVisited, dests))
        return 0;

    std::string bestDest = way[day - 1];
    for (const auto &p : dests) {
        if (currentTime >= maxTime) {
            break;
        }
        way[day - 1] = p;
        const unsigned newPrice = findWay(p, newVisited, way, greedy,
                                          day + 1,
                                          price + timetable[day][ns][p]);
        if (newPrice == 0) {
            continue;
        }
        if (currentPrice == 0 || newPrice < currentPrice) {
            currentPrice = newPrice;
            bestDest = p;
        }
    }
    way[day - 1] = bestDest;
    return currentPrice;
}


static void printTimetable() {
    for (unsigned day = 0; day < N; day++) {
        std::cout << "day: " << day << std::endl;
        for (const auto &ft : timetable[day]) {
            for (const auto &tc : ft.second) {
                std::cout << ft.first << " -> " << tc.first << ": " << tc.second << std::endl;
            }
        }
    }
}


int main() {
    parseInput();
    if (N <= 21)
        maxTime += std::chrono::milliseconds(2500);
    else if (N <= 101)
        maxTime += std::chrono::milliseconds(4500);
    else
        maxTime += std::chrono::milliseconds(14000);
    // printTimetable();
    UniquePlaces visited;
    Way way(N - 1);
    bestWay.resize(N - 1);
    unsigned price = findWay(start, visited, way, false, 1, 0);
    if (!bestPrice) {
        std::cerr << "way not found" << std::endl;
        return 1;
    }
    std::cout << bestPrice << std::endl;
    Airport ns = start;
    for (unsigned day = 1; day < N; day++) {
        const Airport t = bestWay[day - 1];
        std::cout << ns << " " << t << " " << day << " "
                  << timetable[day][ns][t] << std::endl;
        ns = t;
    }
    return 0;
}
