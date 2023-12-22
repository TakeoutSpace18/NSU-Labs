#ifndef INPUTFILEREF_H
#define INPUTFILEREF_H
#include <string>
#include <vector>
#include <boost/any.hpp>

class InputFileRef {
public:
    explicit InputFileRef(const std::string& str);

    [[nodiscard]] std::size_t index() const;
private:
    std::size_t m_index;
};

void validate(boost::any& v,
              const std::vector<std::string>& values,
              InputFileRef* target_type, int);


#endif //INPUTFILEREF_H
