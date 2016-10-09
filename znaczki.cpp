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
/*
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
 */

/**
 * Parses raw line with stamp and inserts it values to retval tuple containing:
 * @param raw_line line to parse if it is of correct format
 * @param retval tuple to place result in there, format:
 * < year, post_office_name / country, < value {string}, value {float}>, stamp_name_index>
 *
 * @return boolean
 * True - if raw_line matches stamp format
 * False - if raw_line doesnt match stamp format
 */
bool parse_stamp(const std::string raw_line,
                 std::tuple<int, std::string, std::pair<std::string, double>, std::string> *retval) {
    std::smatch matches;
    const int stamp_name_regex_index = 1;
    const int stamp_value_regex_index = 3;
    const int stamp_year_regex_index = 5;
    const int post_office_name_regex_index = 6;

    const std::string stamp_format = "^[ ]*" // spaces at the beginning
            "([^ ]+( +[^ ]+)*) +" // stamp name
            "([0-9]+([\\.|,][0-9]+)?) +" // stamp value
            "([0-9]+) +" // stamp year
            "([^ ]*?[^0-9][ ^]*( +[^ ]+)*)" // country of post office name
            " *$"; // spaces at the end

    std::regex expression(stamp_format);
    std::size_t pos;
    if (regex_search(raw_line, matches, expression)) {

        // todo: ogarnac zakresy inta
        std::get<stamp_year_index>(*retval) = boost::lexical_cast<int>(matches[stamp_year_regex_index]);
        std::get<post_office_name_index>(*retval) = matches[post_office_name_regex_index];

        // save value string
        std::string value_string = matches[stamp_value_regex_index];
        std::get<stamp_value_index>(*retval).first = matches[stamp_value_regex_index];
        // save value number
        if ((pos = value_string.find_first_of(',')) != std::string::npos) {
            value_string[pos] = '.';
        }
        std::get<stamp_value_index>(*retval).second = boost::lexical_cast<double>(value_string);

        std::get<stamp_name_index>(*retval) = matches[stamp_name_regex_index];
        return true;
    } else return false;
}

/**
 * Parses line with query
 * @param raw_line line to parse if it is of correct format
 * @param query pointer to a pair where command should be inserted into
 *
 * @return
 * True - if line could be parsed into pair and fulfill constraints
 * False - otherwise
 */
bool parse_query(const std::string raw_line,
                 std::pair<int, int> *query) {
    const std::string query_format = " *([0-9]+) +([0-9]+) *";
    const int lower_bound_regex_index = 1;
    const int upper_bound_regex_index = 2;

    std::smatch matches;
    std::regex expression(query_format);
    if (regex_search(raw_line, matches, expression)) {
        // todo: sprawdz zakresy inta
        query->first = boost::lexical_cast<int>(matches[lower_bound_regex_index]);
        query->second = boost::lexical_cast<int>(matches[upper_bound_regex_index]);

        return query->first <= query->second; //legitimate request only if lower bound is not bigger then upper bound
    } else return false;
}

/**
 * Prints stamp properties to stdout. *
 * @param stamp element to print
 */
void print_stamp(std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp) {
    std::cout
            << std::get<stamp_year_index>(stamp)
            << std::get<post_office_name_index>(stamp)
            << std::get<stamp_value_index>(stamp).first
            << std::get<stamp_name_index>(stamp)
            << std::endl;
}

/**
 * Prints all stamps that match query requirements. *
 * @param query pair with range of years
 * @param stamps set of stamps on which query will be called
 */
void print_stamps(std::pair<int, int> query,
                  std::set<std::tuple<int, std::string, std::pair<std::string, double>, std::string>, compare> stamps) {
    std::pair<std::string, double> p1("", 0);

    std::tuple<int, std::string, std::pair<std::string, double>, std::string> t1 =
            std::make_tuple(query.first, "", p1, "");
    auto it1 = stamps.lower_bound(t1);

    std::tuple<int, std::string, std::pair<std::string, double>, std::string> t2 =
            std::make_tuple(query.second + 1, "", p1, "");
    auto it2 = stamps.lower_bound(t2);

    std::for_each(it1, it2, print_stamp);
}

int main() {
    std::string raw_line;
    std::set<std::tuple<int, std::string, std::pair<std::string, double>, std::string>, compare> stamps;

    bool querying = false;
    const std::string error_message = "Error in line";

    for (int line_count = 1; std::getline(std::cin, raw_line); line_count++) {
        std::tuple<int, std::string, std::pair<std::string, double>, std::string> stamp;
        std::pair<int, int> query;
        if (!querying && parse_stamp(raw_line, &stamp )) { // line is a stamp
            stamps.insert(stamp);
        } else if (parse_query(raw_line, &query)){ // line is a request
            querying = true;
            print_stamps(query, stamps);
        } else {// wrong input
            fprintf(stderr, "%s %d:%s\n", error_message.c_str(), line_count, raw_line.c_str());
        }
    }

    return 0;
}