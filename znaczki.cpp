#include <iostream>
#include <tuple>
#include <set>
#include <algorithm>

#define stamp_key std::tuple<int, std::string, std::pair <std::string, double>, std::string>

int count = 0;

struct compare {
    bool operator()(const stamp_key lhs,
                    const stamp_key rhs) const {
        bool lesser = true;

        lesser = std::get<3>(lhs) < std::get<3>(rhs) ? true :
                 std::get<3>(lhs) > std::get<3>(rhs) ? false : lesser;
        lesser = std::get<2>(lhs).second < std::get<2>(rhs).second ? true :
                 std::get<2>(lhs).second > std::get<2>(rhs).second ? false : lesser;
        lesser = std::get<1>(lhs) < std::get<1>(rhs) ? true :
                 std::get<1>(lhs) > std::get<1>(rhs) ? false : lesser;
        lesser = std::get<0>(lhs) < std::get<0>(rhs) ? true :
                 std::get<0>(lhs) > std::get<0>(rhs) ? false : lesser;

        return lesser;
    }

    bool operator()(const stamp_key lhs,
                    int rhs) {
        return std::get<0>(lhs) < rhs;
    }

    bool operator()(float lhs,
                    const stamp_key rhs) {
        return lhs < std::get<0>(rhs);
    }
};

struct compare_fuck {
    int asYear(const stamp_key &key) const {
        return std::get<0>(key);
    }

    int asYear(int t) const {
        return t;
    }

    template<typename T1, typename T2>
    bool operator()(T1 const &t1,
                    T2 const &t2) const {
        return asYear(t1) < asYear(t2);
    }
};

void insert(stamp_key key,
            std::set<stamp_key, compare> *stamps) {
    (*stamps).insert(key);
}

bool parse_stamp(const std::string raw_line,
                 std::tuple<int, std::string, std::pair<std::string, double>, std::string>
                 *retval) {


}

bool parse_query(const std::string raw_line,
                 std::pair<int, int> *retval) {

}

bool parse_stamp(stamp_key *key) {
    if (count < 5) {
        std::string s1 = "jp2gmd";
        std::pair<std::string, double> p1("tomatoes", 2.30);
        stamp_key t1(2137, s1, p1, s1);
        *key = t1;
        count++;
        return true;
    }
    return false;
}

bool parse_query(std::pair<int, int> *query) {
    if (count < 5) {
        std::pair<int, int> p1(2120, 2140);
        *query = p1;
        ++count;
        return true;
    }
    return false;
}

void print_stamp(stamp_key stamp) {
    std::cout
    << std::get<0>(stamp)
    << std::get<1>(stamp)
    << std::get<2>(stamp).first
    << std::get<3>(stamp)
    << std::endl;
}

void print_stamps(std::pair<int, int> query,
                  std::set<stamp_key, compare> stamps) {
    auto it = std::lower_bound(stamps.begin(), stamps.end(), query.first, compare_fuck());
    auto it2 = std::upper_bound(stamps.begin(), stamps.end(), query.second, compare_fuck());

    std::for_each(it, it2, print_stamp);
}

int main() {
    std::set<stamp_key, compare> stamps;

    std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp;
    std::pair<int, int> query;

    bool inserting;
    bool querying = false;
    std::string raw_line;

    while (std::getline(std::cin, raw_line)) {

        if (!querying) {
            inserting = parse_stamp(raw_line, &stamp);
            if (inserting) {
                insert(stamp, &stamps);
            } else {
                querying = parse_query(raw_line, &query);
                print_stamps(query, stamps);
            }
            if (!querying) {
                fprintf(stderr, "chuj");
            }
        } else {

            querying = parse_query(raw_line, &query);
            if (querying) {
                print_stamps(query, stamps);
            } else {
                fprintf(stderr, "chuj");
            }
        }

    }

    return 0;
}