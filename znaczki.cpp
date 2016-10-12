#include <iostream>
#include <tuple>
#include <set>
#include <algorithm>
#include <regex>
#include <boost/lexical_cast.hpp>

typedef std::pair<std::string, double> stamp_value;
typedef std::tuple<int, std::string, stamp_value, std::string> stamp;

const int stamp_year_index = 0;
const int post_office_name_index = 1;
const int stamp_value_index = 2;
const int stamp_name_index = 3;

/**
 * Comparing function for two stamps.
 * @param lhs left hand side stamp
 * @param rhs right hand side stamp
 * @return
 * True - if left hand side is lesser than right hand side
 * False - othwerise
 */
bool comparer(const stamp lhs, const stamp rhs) {
    bool lesser = false;
    std::string lhs_name = std::get<stamp_name_index>(lhs);
    std::string rhs_name = std::get<stamp_name_index>(rhs);

    double lhs_value = std::get<stamp_value_index>(lhs).second;
    double rhs_value = std::get<stamp_value_index>(rhs).second;

    std::string lhs_office_name = std::get<post_office_name_index>(lhs);
    std::string rhs_office_name = std::get<post_office_name_index>(rhs);

    int lhs_year = std::get<stamp_year_index>(lhs);
    int rhs_year = std::get<stamp_year_index>(rhs);

    lesser = lhs_name < rhs_name ? true : lhs_name > rhs_name ? false : lesser;
    lesser = lhs_value < rhs_value ? true : lhs_value > rhs_value ? false : lesser;
    lesser = lhs_office_name < rhs_office_name ? true : lhs_office_name > rhs_office_name ? false : lesser;
    lesser = lhs_year < rhs_year ? true : lhs_year > rhs_year ? false : lesser;

    return lesser;
}

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
bool parse_stamp(const std::string raw_line, stamp *retval) {
    std::smatch matches;
    const int stamp_name_regex_index = 1;
    const int stamp_value_regex_index = 3;
    const int stamp_year_regex_index = 5;
    const int post_office_name_regex_index = 6;

    const std::string stamp_format = "^[ ]*" // spaces at the beginning
            "([^ ]+( +[^ ]+)*) +" // stamp name
            "([0-9]{1,15}([\\.|,][0-9]+)?) +" // stamp value
            "([0-9]{4}) +" // stamp year
            "([^ ]*?[^0-9][ ^]*( +[^ ]+)*)" // country of post office name
            " *$"; // spaces at the end

    std::regex expression(stamp_format);
    std::size_t pos;
    if (regex_search(raw_line, matches, expression)) {
        // save year and office name
        std::get<stamp_year_index>(*retval) = boost::lexical_cast<int>(matches[stamp_year_regex_index]);
        std::get<post_office_name_index>(*retval) = matches[post_office_name_regex_index];

        // save value string
        std::string value_string = matches[stamp_value_regex_index];
        std::get<stamp_value_index>(*retval).first = matches[stamp_value_regex_index];
        if (value_string[0] == '0' && value_string.size() != 1 &&
            isdigit(value_string[1])) { // trailing zeros are treated as error
            return false;
        }

        // save value number
        if ((pos = value_string.find_first_of(',')) != std::string::npos) {
            value_string[pos] = '.';
        }
        std::get<stamp_value_index>(*retval).second = boost::lexical_cast<double>(value_string);

        // save stamp name
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
bool parse_query(const std::string raw_line, std::pair<int, int> *query) {
    const std::string query_format = " *([0-9]{4}) +([0-9]{4}) *";
    const int lower_bound_regex_index = 1;
    const int upper_bound_regex_index = 2;

    std::smatch matches;
    std::regex expression(query_format);
    if (regex_search(raw_line, matches, expression)) {
        query->first = boost::lexical_cast<int>(matches[lower_bound_regex_index]);
        query->second = boost::lexical_cast<int>(matches[upper_bound_regex_index]);

        return query->first <= query->second; //legitimate request only if lower bound is not bigger then upper bound
    } else return false;
}

/**
 * Prints stamp properties to stdout.
 * @param stamp element to print
 */
void print_stamp(stamp stamp) {
    int year = std::get<stamp_year_index>(stamp);
    std::string post_office = std::get<post_office_name_index>(stamp);
    std::string value = std::get<stamp_value_index>(stamp).first;
    std::string name = std::get<stamp_name_index>(stamp);

    fprintf(stdout, "%d %s %s %s\n", year, post_office.c_str(), value.c_str(), name.c_str());
}

/**
 * Prints all stamps that match query requirements.
 * @param query pair with range of years
 * @param stamps set of stamps on which query will be called
 */
void print_stamps(std::pair<int, int> query, std::set<stamp, bool (*)(stamp, stamp)> stamps) {
    std::pair<std::string, double> pair("", 0);
    stamp lower_bound_tuple = std::make_tuple(query.first, "", pair, "");
    stamp upper_bound_tuple = std::make_tuple(query.second + 1, "", pair, "");
    auto it1 = stamps.lower_bound(lower_bound_tuple);
    auto it2 = stamps.lower_bound(upper_bound_tuple);

    std::for_each(it1, it2, print_stamp);
}

int main() {
    const std::string error_message = "Error in line";
    std::string raw_line;
    bool querying = false;
    bool (*comparer_pointer)(const stamp, const stamp) = comparer;
    std::set<stamp, bool (*)(stamp, stamp)> stamps(comparer_pointer);

    for (int line_count = 1; std::getline(std::cin, raw_line); line_count++) {
        stamp stamp;
        std::pair<int, int> query;

        if (!querying && parse_stamp(raw_line, &stamp)) { // line is a stamp and stamp is not in set already
            stamps.insert(stamp);
        } else if (parse_query(raw_line, &query)) { // line is a request
            querying = true;
            print_stamps(query, stamps);
        } else { // wrong input
            fprintf(stderr, "%s %d:%s\n", error_message.c_str(), line_count, raw_line.c_str());
        }
    }

    return 0;
}