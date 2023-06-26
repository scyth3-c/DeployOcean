#ifndef ROUTE_PROCESS
#define ROUTE_PROCESS

#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include "../HTTP/routes/routes.hpp"

using std::string;

template <class T>
class Worker_t {
private:

    std::atomic<int> MASTER_KEY;

    std::vector<listen_routes> &routes;
    std::vector<std::shared_ptr<T>> &core ;
    std::shared_ptr<HTTP_QUERY> qProcess = nullptr;
    std::condition_variable  &condition;
    std::condition_variable  &condition_response;
    std::vector<std::tuple<std::shared_ptr<T>, std::string>> &worksend;
    inline void _wait(int milis) const { std::this_thread::sleep_for(std::chrono::milliseconds(milis)); }

public:
   Worker_t(std::vector<std::shared_ptr<T>>& _core, std::shared_ptr<HTTP_QUERY>& _qProcess, std::condition_variable& _condition, std::vector<std::tuple<std::shared_ptr<T>, std::string>>& _worksend, std::condition_variable& _condition_response, std::vector<listen_routes>& _routes)
      : MASTER_KEY(1),
      core(_core),
      qProcess(_qProcess),
      condition(_condition),
      worksend(_worksend),
      condition_response(_condition_response),
      routes(_routes)
{
}
    ~Worker_t() {}

    inline auto getWorker(std::mutex &macaco, std::mutex &victor, std::mutex &victoria){

        return [&]()->void{
               while(MASTER_KEY){

                {
                   std::unique_lock<std::mutex> lock(macaco);
                   condition.wait(lock);
                }   

                if(!core.empty()) {   

                string          send_target;
                std::pair<string, string>   actual_route;

                string      parametros{""};
                bool        cantget = true;

                for(auto it=core.begin(); it != core.end();){
                
                std::shared_ptr<T> &control = dup(*it);
                string socket_response {control->getResponse()};
                
                if (socket_response.empty()){
                    throw std::range_error("FAILED TO READ REQUEST, WAIT FEWS SECS BEFORE STARTING AGAIN");
                }

                actual_route = qProcess->route_refactor(socket_response);
     
                for (listen_routes  &it : routes) {

                    if (it.route.getType() == actual_route.first && it.route.getName() == actual_route.second) {

                        parametros = qProcess->route_refactor_params(socket_response);
                        
                        send_target = it.callbacks.execute(parametros);

                        cantget = false;
                        break;

                    }
                }

                std::string sendy {""};

                if (cantget)
                    sendy = ERROR_GET;
                 else 
                    sendy = send_target;

                std::tuple<std::shared_ptr<T>, string> data = std::make_tuple(control, sendy);

                std::lock_guard<std::mutex> guard(victoria);
                worksend.push_back(std::move(data));
                {  condition_response.notify_all(); victor.unlock(); }
    
                it = core.erase(it);
                }
              }
            }
                _wait(10);
        };
    }
};

#endif // !ROUTE_PROCESS