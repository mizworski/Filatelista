#include <iostream>
#include <tuple>
#include <set>
#include <algorithm>

const int year = 0;
const int post_office_name = 1;
const int value = 2;
const int stamp_name = 3;

struct compare {
    bool operator()(const std::tuple<int, std::string, std::pair<std::string, double>, std::string> lhs,
                    const std::tuple<int, std::string, std::pair<std::string, double>, std::string> rhs) const {
        bool lesser = true;

        lesser = std::get<stamp_name>(lhs) < std::get<stamp_name>(rhs) ? true :
                 std::get<stamp_name>(lhs) > std::get<stamp_name>(rhs) ? false : lesser;
        lesser = std::get<value>(lhs).second < std::get<value>(rhs).second ? true :
                 std::get<value>(lhs).second > std::get<value>(rhs).second ? false : lesser;
        lesser = std::get<post_office_name>(lhs) < std::get<post_office_name>(rhs) ? true :
                 std::get<post_office_name>(lhs) > std::get<post_office_name>(rhs) ? false : lesser;
        lesser = std::get<year>(lhs) < std::get<year>(rhs) ? true :
                 std::get<year>(lhs) > std::get<year>(rhs) ? false : lesser;

        return lesser;
    }
};

struct compare_temp {
    int as_year(const std::tuple<int, std::string, std::pair<std::string, double>, std::string> &key) const {
        return std::get<0>(key);
    }

    int as_year(int t) const {
        return t;
    }

    template<typename T1, typename T2>
    bool operator()(T1 const &t1,
                    T2 const &t2) const {
        return as_year(t1) < as_year(t2);
    }
};

/*
bool parse_stamp(const std::string raw_line,
                 std::tuple<int, std::string, std::pair<std::string, double>, std::string>
                 *retval) {

}

bool parse_query(const std::string raw_line,
                 std::pair<int, int> *retval) {

}
*/

void print_stamp(std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp) {
    std::cout
    << std::get<year>(stamp)
    << std::get<post_office_name>(stamp)
    << std::get<value>(stamp).first
    << std::get<stamp_name>(stamp)
    << std::endl;
}

void print_stamps(std::pair<int, int> query,
                  std::set<std::tuple<int, std::string, std::pair<std::string, double>, std::string>, compare> stamps) {
    auto it = std::lower_bound(stamps.begin(), stamps.end(), query.first, compare_temp());
    auto it2 = std::upper_bound(stamps.begin(), stamps.end(), query.second, compare_temp());

    std::for_each(it, it2, print_stamp);
}

int main() {
    std::set<std::tuple<int, std::string, std::pair<std::string, double>, std::string>, compare> stamps;
    std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp;
    std::pair<int, int> query;
    bool inserting;
    bool querying = false;
    std::string raw_line;

    while (std::getline(std::cin, raw_line)) {
        if (!querying) {
            inserting = parse_stamp(raw_line, &stamp);
            if (inserting) {
                stamps.insert(stamp);
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