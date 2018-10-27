#!/usr/bin/env python3
import sys

INF = float("inf")

def parse_input():
    global areas, start, airports, timetable, N

    areas = dict()
    airports = dict()

    fstline = sys.stdin.readline().strip().split()
    N = int(fstline[0]) + 1
    start = fstline[1]

    for _ in range(N - 1):
        area = sys.stdin.readline().strip()
        airports[area] = sys.stdin.readline().strip().split()
        for a in airports[area]:
            areas[a] = area

    timetable = [dict() for _ in range(N)]
    for line in sys.stdin:
        f, t, d, c = line.strip().split()
        if areas[f] is areas[t]:
            continue
        d, c = [int(d)], int(c)
        if d[0] == 0:
            sd = 1 if f == start else N - 1 if areas[t] is areas[start] else 2 
            d = range(sd, N)
        for day in d:
            if day == N - 1 and areas[t] is not areas[start]:
                continue
            dt = timetable[day]
            if f not in dt:
                dt[f] = dict()
            if t not in dt[f] or dt[f][t] > c:
                dt[f][t] = c


def possible_airports(day, frm, visited_areas):
    global areas, start, airports, timetable, N
    if frm not in timetable[day]:
        return list()
    if day < N - 1:
        ports = [p for p in timetable[day][frm].keys() if areas[p] not in visited_areas]
    else:
        ports = [p for p in timetable[day][frm].keys()] # there should be only ways to the last area
    return sorted(ports, key=lambda t: timetable[day][frm][t])



def find_way(ns, visited_areas=[], way=[], day=1, price=0):
    global areas, start, airports, timetable, N

    if day == N - 1:
        ports = possible_airports(day, ns, visited_areas)

        if len(ports) < 1:
            return []
        way.append(ports[0])
        price += timetable[day][ns][way[-1]]
        return way

    visited_areas.append(areas[ns])
    ports = possible_airports(day, ns, visited_areas)
    if len(ports) < 1:
        return []
    for p in ports:
        nprice = price + timetable[day][ns][p]
        nway = way + [p]
        w = find_way(p, visited_areas, nway, day + 1, nprice)
        if len(w) > 0:
            return w


def main():
    global areas, start, airports, timetable
    parse_input()

    price = 0
    way = find_way(start, [], [], 1, price)
    day = 1
    ns = start
    for t in way:
        price += timetable[day][ns][t]
        ns = t
        day += 1
    print(price)

    day = 1
    ns = start
    for t in way:
        print("{} {} {} {}".format(ns, t, day, timetable[day][ns][t]))
        day += 1
        ns = t


if __name__ == '__main__':
    sys.setrecursionlimit(400)
    main()
