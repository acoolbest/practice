#ifndef _JSON_READ_H_
#define _JSON_READ_H_

#include <sstream>
#include "third_party/libjson/libjson.h"
#include "def.h"

int read_config(user_config& cnfg);
int read_net_traffic(net_traffic_stru & cnfg);
int write_net_traffic(net_traffic_stru cnfg);



#endif

