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

namespace utility_workers {
    template <class T>
    class Worker_t {
    private:

        std::atomic<int> MASTER_KEY = 1;

        std::vector<listen_routes> &routes;
        std::vector<std::shared_ptr<T>> &core;
        std::shared_ptr<HTTP_QUERY> qProcess = nullptr;
        std::condition_variable  &condition;
        std::condition_variable  &condition_response;
        std::vector<std::tuple<std::shared_ptr<T>, std::string>> &worksend;

        inline void _wait(int milis) const { std::this_thread::sleep_for(std::chrono::milliseconds(milis)); }

    public:

        explicit Worker_t(std::vector<std::shared_ptr<T>>& _core, std::shared_ptr<HTTP_QUERY>& _qProcess, std::condition_variable& _condition, std::vector<std::tuple<std::shared_ptr<T>, std::string>>& _worksend, std::condition_variable& _condition_response, std::vector<listen_routes>& _routes)
        : core(_core),
        qProcess(_qProcess),
        condition(_condition),
        worksend(_worksend),
        condition_response(_condition_response),
        routes(_routes)
        {}
        ~Worker_t() {}


        inline auto getWorker(std::mutex &macaco, std::mutex &victor, std::mutex &victoria){

            return [&]()->void{
                while(MASTER_KEY){

                    {
                        std::unique_lock<std::mutex> lock(macaco);
                        condition.wait(lock);
                    }

                    if(!core.empty()) {

                        std::pair<string, string>   actual_route;

                        for(auto it=core.begin(); it != core.end();){

                            auto &control = *it;
                            bool cantget = true;

                            std::string send_target     {""};
                            std::string socket_response {control->getResponse()};

                            if (socket_response.empty()){
                                throw std::range_error("FAILED TO READ REQUEST, WAIT FEWS SECS BEFORE STARTING AGAIN");
                            }

                            actual_route = qProcess->route_refactor(socket_response);

                            for (auto &it : routes) {

                                std::cout.clear();

                                if (it.route.getType() == actual_route.first && it.route.getName() == actual_route.second) {

                                    send_target = it.callbacks.execute(qProcess->route_refactor_params(socket_response));
                                    cantget = false;

                                    break;

                                }
                            }

                            std::string sendy = cantget ? ERROR_GET : send_target;
                            auto data = std::make_tuple(control, sendy);
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
}

#endif // !ROUTE_PROCESS