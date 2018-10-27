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
                        Way &way, const bool greedy = true,
                        const unsigned day = 1, const unsigned price = 0) {
    std::vector<Airport> dests;
    dests.reserve(16);
    if (day == N - 1) {
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

    Way newWay(way);
    const unsigned lastElementId = newWay.size() + 1;
    newWay.resize(lastElementId + 1);
    for (const auto &p : dests) {
        newWay[lastElementId] = p;
        const unsigned newPrice = findWay(p, newVisited, newWay, greedy,
                                          day + 1,
                                          price + timetable[day][ns][p]);
        if (newPrice == 0)
            continue;
        way = std::move(newWay);
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
    way.reserve(N);
    unsigned price = findWay(start, visited, way, true);
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
