#ifndef NEODIMIO_HPP
#define NEODIMIO_HPP

#include "TCP-IP/mgsockets.h"
#include "utils/notify.h"

#include "HTTP/routes/routes.hpp"
#include "HTTP/request/request.hpp"

#include "processing/parameters/parameter_proccess.h"
#include "workers/route_process.hpp"
#include "workers/send_process.h"

#include "utils/enums.h"
#include "utils/sysprocess.h"


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

using workers::Worker_t;
using workers::Send_t;

constexpr int BUFFER = enums::neo::eSize::BUFFER;
constexpr int SESSION = enums::neo::eSize::SESSION;

template <class T>
class Neody {
private:

    Worker_t<T> *Worker_maestro = nullptr;
    Send_t<T> *Send_maestro = nullptr;

    std::vector<std::shared_ptr<T>> worker_one;
    
    std::vector<std::tuple<std::shared_ptr<T>, string>> worker_send;
    std::vector<listen_routes> routes;

    std::shared_ptr<T> control = nullptr;
    std::shared_ptr<HTTP_QUERY> qProcess = nullptr;

    std::mutex victoria;
    std::mutex victor;
    std::mutex macaco;

    std::condition_variable condition_one;
    std::condition_variable condition_response;
    std::condition_variable condition_access;

    uint16_t PORT{enums::neo::eSize::DEF_PORT};

    std::atomic<int> next_register =  enums::neo::DEF_REG;
    std::atomic<bool> MASTER_KEY = enums::neo::eSize::SESSION;

public:
    [[maybe_unused]] [[maybe_unused]]  explicit Neody(uint16_t port);
    explicit Neody();
    ~Neody() {
        CLOSE();
        control.reset();
        qProcess.reset();
    }

    inline void CLOSE() noexcept { MASTER_KEY = false;  }

    int http_response(string, _callbacks, string optional);
    [[maybe_unused]] int get(string, _callbacks);
    [[maybe_unused]] int post(string, _callbacks);
    [[maybe_unused]] int put(string, _callbacks);
    [[maybe_unused]] int deleteX(string, _callbacks);
    [[maybe_unused]] int patch(string, _callbacks);
    [[maybe_unused]] int head(string, _callbacks);
    [[maybe_unused]] int options(string, _callbacks);
    [[maybe_unused]] int link(string, _callbacks);
    [[maybe_unused]] int unlink(string, _callbacks);
    [[maybe_unused]] int purge(string, _callbacks);
    
    int setPort(uint16_t) noexcept;
    void listen();
    void add_queue(std::shared_ptr<T> &);
};

template <class T>
[[maybe_unused]] Neody<T>::Neody(uint16_t port) {
    if (port >= enums::neo::eSize::MIN_PORT) { PORT = port; }
    Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
    Send_maestro = new Send_t(worker_send, condition_response);
}

template <class T>
Neody<T>::Neody() { 
     Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
     Send_maestro = new Send_t(worker_send, condition_response);
}

template <class T>

int Neody<T>::http_response(string _xRoute, _callbacks _funcs, string optional_type) {
    try {
        routes.emplace_back(std::move(_xRoute), std::move(_funcs), std::move(optional_type));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return enums::neo::eReturn::ERROR;
    }
    return enums::neo::eReturn::OK;
}

template <class T>
[[maybe_unused]] int Neody<T>::get(string _xRoute, _callbacks _funcs){
    return http_response(_xRoute, _funcs, GET_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::post(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, POST_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::put(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PUT_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::deleteX(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, DELETE_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::patch(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PATCH_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::head(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, HEAD_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::options(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, OPTIONS_TYPE);
}
template <class T>
[[maybe_unused]]  int Neody<T>::link(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, LINK_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::unlink(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, UNLINK_TYPE);
}
template <class T>
[[maybe_unused]] int Neody<T>::purge(string _xRoute, _callbacks _funcs) {
    return http_response(_xRoute, _funcs, PURGE_TYPE);
}

template <class T>
void Neody<T>::listen() {

    auto listen_loop_MAIN = [&]() -> void {
        while (MASTER_KEY){
            try {

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
                add_queue(control);
            }
            catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
     }
};

    auto worker = Worker_maestro->getWorker(macaco, victor, victoria);
    auto Sender = Send_maestro->getSendProcess(victor);

    std::thread _worker(worker);
    std::thread _response(Sender);
    std::thread _sender(listen_loop_MAIN);

    _sender.join();
    _response.join();
    _worker.join();
}

template<class T> void Neody<T>::add_queue(std::shared_ptr<T> &base) {
    switch (next_register.load()) {
    case 0:
            worker_one.push_back(base);
            {  condition_one.notify_all(); macaco.unlock(); }
            // next_register.store(next_register.load() + 1);
       break;
    default:
        break;
    }
}

template <class T>
int Neody<T>::setPort(uint16_t _port) noexcept {
    if (_port >= enums::neo::eSize::MIN_PORT) { PORT = _port; }
    else
        return enums::neo::eReturn::ERROR;
    return enums::neo::eReturn::OK;
}
typedef Neody<Server> Router;

#endif // NEODIMIO_HPP