#include "Location.h"
#include <sstream>

namespace CHTL {

std::string Location::ToString() const {
    std::stringstream ss;
    if (!filename_.empty()) {
        ss << filename_ << ":";
    }
    ss << line_ << ":" << column_;
    return ss.str();
}

std::string Range::ToString() const {
    if (start_.GetFilename() == end_.GetFilename()) {
        std::stringstream ss;
        if (!start_.GetFilename().empty()) {
            ss << start_.GetFilename() << ":";
        }
        ss << start_.GetLine() << ":" << start_.GetColumn() 
           << "-" << end_.GetLine() << ":" << end_.GetColumn();
        return ss.str();
    } else {
        return start_.ToString() + " - " + end_.ToString();
    }
}

} // namespace CHTL