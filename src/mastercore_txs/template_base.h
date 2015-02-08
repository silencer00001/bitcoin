#ifndef _MASTERCORE_TEMPLATE_BASE
#define _MASTERCORE_TEMPLATE_BASE

#include <string>
#include <vector>
#include "json/json_spirit_value.h"

namespace mastercore {
namespace transaction {

class CTemplateBase
{
public:
    virtual bool Deserialize(const std::vector<unsigned char>& vch) = 0;
    virtual std::vector<unsigned char> Serialize() const = 0;
    virtual void Serialize(std::vector<unsigned char>& vch) const = 0;
    virtual void Serialize(json_spirit::Object& obj) const = 0;
    virtual ~CTemplateBase() {};
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_TEMPLATE_BASE
