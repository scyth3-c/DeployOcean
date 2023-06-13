#ifndef ROUTE_PROCESS
#define ROUTE_PROCESS

#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <condition_variable>
#include "../HTTP/routes/routes.hpp"

template <class T>
class Worker_t {
private:

    std::vector<listen_routes>  sesion_routes;
    shared_ptr<string>          send_target;
    std::pair<string, string>   actual_route;

    string    parametros{""};
    bool      cantget = true;

    std::vector<listen_routes> &routes;
    std::vector<std::shared_ptr<T>> &core = nullptr;
    std::shared_ptr<HTTP_QUERY> qProcess = nullptr;
    std::condition_variable  &condition;
    std::condition_variable  &condition_response;
    std::vector<std::tuple<std::shared_ptr<T>, std::string>> &worksend;


public:
    Worker_t(std::vector<std::shared_ptr<T>> &_core, std::shared_ptr<HTTP_QUERY> &_qProcess, std::condition_variable &_condition, std::vector<std::tuple<std::shared_ptr<T>, std::string>> _worksend, std::condition_variable &_condition_response, std::vector<listen_routes> &_routes) : routes(_routes), worksend(_worksend), core(_core), qProcess(_qProcess), condition(_condition), condition_response(_condition_response) {}
    ~Worker_t() {}

    inline auto Execute(std::mutex &macaco, std::mutex &victor, std::mutex &victoria){

        return [&]()->void {
            while(1){
             {
                std::unique_lock<std::mutex> lock(macaco);
                condition.wait(lock);
             }
        }

        bool base = core.empty();
        std::cout << base << std::endl;
       

        if (core.empty()){

                std::cout << "core" << std::endl;

             for(auto it=core.begin(); it != core.end();){

                std::shared_ptr<Server> &control = *it;
                send_target = make_shared<string>();

                string socket_response {control->getResponse()};

                if (socket_response.empty()){
                    throw std::range_error("FAILED TO READ REQUEST, WAIT FEWS SECS BEFORE STARTING AGAIN");
                }
                actual_route = qProcess->route_refactor(socket_response);
                sesion_routes = routes; // generate COPY  NOT MOVE X

                for (auto &it : sesion_routes) {
             
                    if (it.route.getType() == actual_route.first && it.route.getName() == actual_route.second) {
                        parametros = qProcess->route_refactor_params(socket_response);
                        send_target.reset(new string(it.callbacks.execute(parametros)));
                        cantget = false;
                        break;
                    }
                }

                std::string sendy = cantget ? ERROR_GET : *send_target; 
                auto data = std::make_tuple(control, sendy);

                std::lock_guard<std::mutex> guard(victoria);
                worksend.push_back(std::move(data));

                {  condition_response.notify_all(); victor.unlock(); }

                it = core.erase(it);
                }   
        }
    };
    }
};


#endif // !ROUTE_PROCESS