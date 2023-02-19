#ifndef UTILY_H
#define UTILY_H
#include <common.h>
#include "json/json.h"

namespace ns_utily {
using namespace std;
using namespace Json;

inline bool ParseJson(const string &strJsn, Value &jsn) {
	Reader rd;
	return rd.parse(strJsn, jsn);
}
};
#endif
