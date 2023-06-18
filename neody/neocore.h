#ifndef NEODIMIO_HPP
#define NEODIMIO_HPP

#include "TCP-IP/mgsockets.h"
#include "utils/notify.h"

#include "HTTP/routes/routes.hpp"
#include "HTTP/request/request.hpp"

#include "processing/parameters/parameter_proccess.h"
#include "workers/route_process.hpp"

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <memory>

#include <string>
#include <vector>
#include <stdexcept>
#include <queue>
#include <atomic>
#include <tuple>
#include <ios>

using std::make_shared;
using std::string;

constexpr int BUFFER = 2048;
constexpr int SESSION = 1;


template <class T>
class Neody {
private:

    Worker_t<T> *Worker_maestro = nullptr;

    std::vector<std::shared_ptr<T>> worker_one;
    std::vector<std::shared_ptr<T>> worker_two;
    std::vector<std::shared_ptr<T>> worker_three;
    
    std::vector<std::tuple<std::shared_ptr<T>, string>> worker_send;
    std::vector<listen_routes> routes;

    std::shared_ptr<T> control = nullptr;
    std::shared_ptr<HTTP_QUERY> qProcess = nullptr;

    std::mutex victoria;
    std::mutex victor;
    std::mutex macaco;
    std::mutex sockety;

    std::condition_variable condition_one;
    std::condition_variable condition_tow;
    std::condition_variable condition_three;
    std::condition_variable condition_response;
    std::condition_variable condition_access;


    uint16_t PORT{0};

    unsigned short int _domain = AF_INET;
    unsigned short int type = SOCK_STREAM;
    unsigned short int protocol = 0;
    unsigned short int route_seer = 0;
    unsigned short int max_iterator = 50;

    std::atomic<int> next_register = 0;
    std::atomic<bool> MASTER_KEY = true;

public:
    Neody(uint16_t port);
    Neody();
    ~Neody() {
        CLOSE();
        worker_one.clear();
        worker_two.clear();
        worker_three.clear();
        worker_send.clear();
        control.reset();
        qProcess.reset();
    }

    inline void _wait(int milis) const { std::this_thread::sleep_for(std::chrono::milliseconds(milis)); }
    inline void CLOSE() { MASTER_KEY = false;  }

    int http_response(string, _callbacks, string optional);
    int get(string, _callbacks);
    int post(string, _callbacks);
    int put(string, _callbacks);
    int deleteX(string, _callbacks);
    int patch(string, _callbacks);
    int head(string, _callbacks);
    int options(string, _callbacks);
    int link(string, _callbacks);
    int unlink(string, _callbacks);
    int purge(string, _callbacks);
    
    int setPort(uint16_t) noexcept;
    void listen();
    void add_queue(std::shared_ptr<T> &&);
};

template <class T>
Neody<T>::Neody(uint16_t port) {
    if (port >= 100) {
        PORT = std::move(port);
    }
    Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
}
template <class T>
Neody<T>::Neody() { 
     Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
 }


template <class T>

int Neody<T>::http_response(string _xRoute, _callbacks _funcs, string optional_type) {
    try {
        routes.push_back({std::move(_xRoute), std::move(_funcs), std::move(optional_type)});
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
    return 0;
}



template <class T>
int Neody<T>::get(string _xRoute, _callbacks _funcs){
    return http_response(_xRoute, _funcs, GET_TYPE);
}
template <class T>
int Neody<T>::post(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, POST_TYPE);
}
template <class T>
int Neody<T>::put(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PUT_TYPE);
}
template <class T>
int Neody<T>::deleteX(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, DELETE_TYPE);
}
template <class T>
int Neody<T>::patch(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PATCH_TYPE);
}
template <class T>
int Neody<T>::head(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, HEAD_TYPE);
}
template <class T>
int Neody<T>::options(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, OPTIONS_TYPE);
}
template <class T>
int Neody<T>::link(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, LINK_TYPE);
}
template <class T>
int Neody<T>::unlink(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, UNLINK_TYPE);
}
template <class T>
int Neody<T>::purge(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PURGE_TYPE);
}

template <class T>
void Neody<T>::listen() {


        
            auto process_two =   [&]() -> void {
                 while(MASTER_KEY){

                {
                   std::unique_lock<std::mutex> lock(macaco);
                   condition_tow.wait(lock);
                }   

                if(!worker_two.empty()) {   

    
                std::vector<listen_routes>  sesion_routes;
                shared_ptr<string>          send_target;
                std::pair<string, string>   actual_route;
                

                string      parametros{""};
                bool        cantget = true;

                for(auto it=worker_two.begin(); it != worker_two.end();){
                
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
                worker_send.push_back(std::move(data));
                {  condition_response.notify_all(); victor.unlock(); }
       
                it = worker_two.erase(it);

                }
              }
            }
                _wait(10);
               
            };



            auto process_three = [&]() -> void {
               while(MASTER_KEY){
                 {
                   std::unique_lock<std::mutex> lock(macaco);
                   condition_three.wait(lock);
                 }

                if(!worker_three.empty()) {   

                std::vector<listen_routes>  sesion_routes;
                shared_ptr<string>          send_target;
                std::pair<string, string>   actual_route;
            
                string      parametros{""};
                bool        cantget = true;

                for(auto it=worker_three.begin(); it != worker_three.end();){
                
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
                worker_send.push_back(std::move(data));

                {  condition_response.notify_all(); victor.unlock(); }
              
                it = worker_three.erase(it);

                }
              }
            }
                _wait(10);
            };


    auto hilo_envia = [&]() -> void {
       while(MASTER_KEY){
          {
                std::unique_lock<std::mutex> lock(victor);
                condition_response.wait(lock);
          }

         if(!worker_send.empty()){

            for(auto it = worker_send.begin(); it != worker_send.end();){
                     try {
                           auto &[sender, data] = *it;         
              
                            sender->sendResponse(data);
                            if(close(sender->getDescription()) < 0) {
                                std::range_error("ERROR CLOSE SOCKET");
                            }
                            it = worker_send.erase(it);
                     }
                     catch(const std::exception& e) {
                        std::cerr << e.what() << '\n';
                        }
                    }
            }
            _wait(10);
       }
    };


    auto listen_loop_MAIN = [&]() -> void {
        while (MASTER_KEY){
            try {
                qProcess = make_shared<HTTP_QUERY>();
                control = make_shared<T>();

                if (!control->create()) {
                    std::range_error("error al crear");
                }

                control->setBuffer(BUFFER);
                control->setPort(PORT);
                control->setSessions(SESSION);

                if (!control->on()){
                    std::range_error("error al lanzar");
                }
             
                add_queue(std::move(control));
            }
            catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
     }
};
    std::thread _sender(listen_loop_MAIN);
    std::thread _response(hilo_envia);

    auto worker = Worker_maestro->getWorker(macaco, victor, victoria);
    std::thread procesador_base(worker);

    // std::thread two_worker(process_two);
    // std::thread three_worker(process_three);

    _sender.join();
    _response.join();
    
    procesador_base.join();
    // two_worker.join();
    // three_worker.join();
}

template<class T> void Neody<T>::add_queue(std::shared_ptr<T> &&base) {
    switch (next_register) {
    case 0:
            worker_one.push_back(std::move(base));
            {  condition_one.notify_all(); macaco.unlock(); }
            // next_register++;
        break;
    case 1:
            worker_two.push_back(std::move(base));
             {  condition_tow.notify_all();  macaco.unlock();}
            next_register++;
        break;
    case 2:
            worker_three.push_back(std::move(base));
             {  condition_three.notify_all(); macaco.unlock(); }
            next_register = 0;
        break;
    default:
        break;
    }
}



template <class T>
int Neody<T>::setPort(uint16_t _port) noexcept {
    if (_port >= 100) { PORT = std::move(_port); }
    else {
        return -1;
    }
    return 0;
}


typedef Neody<Server> Router;

#endif // !Neody_HPP