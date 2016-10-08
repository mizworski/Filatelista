#include <iostream>
#include <tuple>
#include <set>
#include <algorithm>
#include <regex>
#include <boost/lexical_cast.hpp>

const int stamp_year_index = 0;
const int post_office_name_index = 1;
const int stamp_value_index = 2;
const int stamp_name_index = 3;

using namespace boost;

struct compare {
    bool operator()(const std::tuple<int, std::string, std::pair<std::string, double>, std::string> lhs,
                    const std::tuple<int, std::string, std::pair<std::string, double>, std::string> rhs) const {
        bool lesser = true;

        lesser = std::get<stamp_name_index>(lhs) < std::get<stamp_name_index>(rhs) ? true :
                 std::get<stamp_name_index>(lhs) > std::get<stamp_name_index>(rhs) ? false : lesser;
        lesser = std::get<stamp_value_index>(lhs).second < std::get<stamp_value_index>(rhs).second ? true :
                 std::get<stamp_value_index>(lhs).second > std::get<stamp_value_index>(rhs).second ? false : lesser;
        lesser = std::get<post_office_name_index>(lhs) < std::get<post_office_name_index>(rhs) ? true :
                 std::get<post_office_name_index>(lhs) > std::get<post_office_name_index>(rhs) ? false : lesser;
        lesser = std::get<stamp_year_index>(lhs) < std::get<stamp_year_index>(rhs) ? true :
                 std::get<stamp_year_index>(lhs) > std::get<stamp_year_index>(rhs) ? false : lesser;

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
/**
 * Parses raw line with stamp and inserts it values to retval tuple containing:
 * @param raw_line line with stamp data
 * @param retval tuple to place result in there, format:
 * < year, post_office_name / country, < value {string}, value {float}>, stamp_name_index>
 *
 * @return boolean
 * true - if raw_line matches stamp format
 * false - if raw_line doesnt match stamp format
 */
bool parse_stamp(const std::string raw_line,
                 std::tuple<int, std::string, std::pair <std::string, double>, std::string>* retval){
    std::smatch matches;
    const int stamp_name_regex_index = 1;
    const int stamp_value_regex_index = 3;
    const int stamp_year_regex_index = 5;
    const int post_office_name_regex_index = 6;

    const std::string stamp_format = "^[ ]*" // spaces at the beginning
            "([^ ]+( +[^ ]+)*) +" // stamp name
            "([0-9]+([\\.|,][0-9]+)?) +" // stamp value
            "([0-9]+) +" // stamp year
            "([^ ]*?[^0-9][ ^]*)" // country of post office name
            " *$"; // spaces at the end

    std::regex expression(stamp_format);

    if(regex_search(raw_line, matches, expression)){

        // todo: ogarnac zakresy inta
        std::get<stamp_year_index>(*retval) = boost::lexical_cast<int>(matches[stamp_year_regex_index]);
        std::get<post_office_name_index>(*retval) = matches[post_office_name_regex_index];
        std::get<stamp_value_index>(*retval).first = matches[stamp_value_regex_index];
        // todo: zwroc wartosc z napisu

        std::get<stamp_name_index>(*retval) = matches[stamp_name_regex_index];
        return true;
    }
    else return false;
}

/**
 * Parse line with query
 * @param raw_line line with query command
 * @param query pointer to a pair where command should be inserted into
 *
 * @return
 * True - if line could be parsed into pair and fulfill constraints
 * False - otherwise
 */
bool parse_query(const std::string raw_line,
                 std::pair<int, int> *query){
    const std::string query_format = " *[0-9]+ + [0-9]+ *";
    const lower_bound_regex_index = 1;
    const upper_bound_regex_index = 2;

    std::smatch matches;
    std::regex expression(query_format);
    if(regex_search(raw_line, matches, expression)){
        // todo: sprawdz zakresy inta
        query->first = boost::lexical_cast<int>(matches[lower_bound_regex_index]);
        query->second = boost::lexical_cast<int>(matches[upper_bound_regex_index]);

        return query->first <= query->second; //legitimate request only if lower bound is not bigger then upper bound
    }
    else return false;
}


void print_stamp(std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp) {
    std::cout
    << std::get<stamp_year_index>(stamp)
    << std::get<post_office_name_index>(stamp)
    << std::get<stamp_value_index>(stamp).first
    << std::get<stamp_name_index>(stamp)
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