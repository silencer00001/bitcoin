#ifndef _MASTERCORE_TEMPLATE_BASE
#define _MASTERCORE_TEMPLATE_BASE

//#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>
#include <vector>
#include "json/json_spirit_value.h"

namespace mastercore {
namespace transaction {

class CTemplateBase
{
public:
    virtual bool Deserialize(const std::vector<unsigned char>& _vch) = 0;
    virtual std::vector<unsigned char> Serialize() = 0;
    virtual void Serialize(std::vector<unsigned char>& _vch) = 0;
    virtual void Serialize(json_spirit::Object& _obj) = 0;
    virtual ~CTemplateBase() {};
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_TEMPLATE_BASE
