#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <algorithm>

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


// static unsigned
// 

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
            startDay = (f == start) ? 1 : (areas[t] == areas[start]) ? N - 1 : 2;
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
                             std::vector<Airport> &dests) {
    auto el = timetable[day].find(from);
    if (el == timetable[day].end())
        return false;
    dests.clear();
    for (const auto &tc : el->second) {
        if (day < N - 1 && visited.find(areas[tc.first]) != visited.end())
            continue;
        dests.push_back(tc.first);
    }
    std::sort(dests.begin(), dests.end(),[&](const Airport &a, const Airport &b) {
        return timetable[day][from][a] < timetable[day][from][b];
    });
    return !dests.empty();
}

static unsigned findWay(const Airport &ns, UniquePlaces &visited,
                        Way &way, const unsigned day, const unsigned price) {
    std::vector<Airport> dests;
    if (day == N -1) {
        if (!possibleAirports(day, ns, visited, dests))
            return 0;
        Airport bestEnd = dests.front();
        way.push_back(bestEnd);
        return price + timetable[day][ns][bestEnd];
    }
    UniquePlaces newVisited(visited);
    newVisited.emplace(areas[ns]);
    if (!possibleAirports(day, ns, newVisited, dests))
        return 0;
    for (const auto &p : dests) {
        unsigned newPrice = price + timetable[day][ns][p];
        Way newWay(way);
        newWay.push_back(p);
        newPrice = findWay(p, newVisited, newWay, day + 1, newPrice);
        if (newPrice == 0)
            continue;
        way = newWay;
        return newPrice;
    }
    return 0;
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
    // printTimetable();
    UniquePlaces visited;
    Way way;
    unsigned price = findWay(start, visited, way, 1, 0);
    if (!price) {
        std::cerr << "way not found" << std::endl;
        return 1;
    }
    std::cout << price << std::endl;
    Airport ns = start;
    for (unsigned day = 1; day < N; day++) {
        const Airport t = way[day - 1];
        std::cout << ns << " " << t << " " << day << " "
                  << timetable[day][ns][t] << std::endl;
        ns = t;
    }
    return 0;
}
