#include "Token.h"
#include <sstream>

namespace CHTL {

std::string Token::ToString() const {
    std::stringstream ss;
    ss << "Token(" << static_cast<int>(type_) << ", \"" << lexeme_ << "\", " 
       << location_.ToString() << ")";
    return ss.str();
}

} // namespace CHTL