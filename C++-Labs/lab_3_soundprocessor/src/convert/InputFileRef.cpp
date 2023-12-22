#include "InputFileRef.h"

#include <regex>
#include <boost/program_options.hpp>


InputFileRef::InputFileRef(const std::string& str)
{
    m_index = std::stoi(str.substr(1));
}

std::size_t InputFileRef::index() const
{
    return m_index;
}

void validate(boost::any& v,
              const std::vector<std::string>& values,
              InputFileRef* target_type, int)
{
    namespace po = boost::program_options;

    // regex for validating "$<index> reference format"
    static std::regex r("\\$\\d+");

    // Make sure no previous assignment to 'a' was made.
    po::validators::check_first_occurrence(v);

    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const std::string& s = po::validators::get_single_string(values);

    if (std::smatch match; regex_match(s, match, r)) {
        v = boost::any(InputFileRef(match[0]));
    }
    else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}
