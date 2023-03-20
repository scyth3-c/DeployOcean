#ifndef SOCKETS_WRAPPER
#define SOCKETS_WRAPPER


#include <memory>
#include <mutex>
#include <string>
#include <functional>
#include <vector>
#include <tuple>

#include "../TCP-IP/mgsockets.h"

using std::string;
using std::vector;

class SOCKET {

private:

  std::unique_ptr<Server> SOCKET;
  
  uint16_t llave;
  bool estate;
  std::vector<std::tuple<int, string>> response_stream;

public:

   bool on();
   bool send();
   bool get();

};

#endif //SOCKETS_WRAPPER