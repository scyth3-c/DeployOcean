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
#include <assert.h>
#include <functional>

using std::make_shared;
using std::string;

namespace neo
{

    using utility_workers::Worker_t;

    constexpr int BUFFER = 2048;
    constexpr int SESSION = 1;

    template <class T>
    class Neody
    {
    private:
        Worker_t<T> *Worker_maestro = nullptr;

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

        uint16_t PORT{0};

        std::atomic<int> next_register = 0;
        std::atomic<bool> MASTER_KEY = true;

    public:
        explicit Neody(uint16_t port);
        explicit Neody();
        ~Neody()
        {
            CLOSE();
            control.reset();
            qProcess.reset();
        }

        inline void _wait(int milis) const { std::this_thread::sleep_for(std::chrono::milliseconds(milis)); }
        inline void CLOSE() { MASTER_KEY = false; }

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
        void add_queue(std::shared_ptr<T> &);
    };

    template <class T>
    Neody<T>::Neody(uint16_t port)
    {
        if (port >= 100)
        {
            PORT = std::move(port);
        }
        Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
    }
    template <class T>
    Neody<T>::Neody()
    {
        Worker_maestro = new Worker_t(worker_one, qProcess, condition_one, worker_send, condition_response, routes);
    }

    template <class T>

    int Neody<T>::http_response(string _xRoute, _callbacks _funcs, string optional_type)
    {
        try
        {
            routes.push_back({std::move(_xRoute), std::move(_funcs), std::move(optional_type)});
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        return 0;
    }

    template <class T>
    int Neody<T>::get(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, GET_TYPE);
    }
    template <class T>
    int Neody<T>::post(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, POST_TYPE);
    }
    template <class T>
    int Neody<T>::put(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, PUT_TYPE);
    }
    template <class T>
    int Neody<T>::deleteX(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, DELETE_TYPE);
    }
    template <class T>
    int Neody<T>::patch(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, PATCH_TYPE);
    }
    template <class T>
    int Neody<T>::head(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, HEAD_TYPE);
    }
    template <class T>
    int Neody<T>::options(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, OPTIONS_TYPE);
    }
    template <class T>
    int Neody<T>::link(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, LINK_TYPE);
    }
    template <class T>
    int Neody<T>::unlink(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, UNLINK_TYPE);
    }
    template <class T>
    int Neody<T>::purge(string _xRoute, _callbacks _funcs)
    {
        return http_response(_xRoute, _funcs, PURGE_TYPE);
    }

    template <class T>
    void Neody<T>::listen()
    {

        std::function<void(void)> hilo_envia = [&]() -> void
        {
            while (MASTER_KEY)
            {
                {
                    std::unique_lock<std::mutex> lock(victor);
                    condition_response.wait(lock);
                }

                if (!worker_send.empty()) {
                    std::cout << std::flush;
                    for (auto it = worker_send.begin(); it != worker_send.end();) {
                        
                        try {
                            auto &[sender, data] = *it;

                            std::cout << data << std::endl;

                            sender->sendResponse(data);
                            if (close(sender->getDescription()) < 0) {
                                std::range_error("ERROR CLOSE SOCKET");
                            }
                            it = worker_send.erase(it);
                        }
                        catch (const std::exception &e) {
                            std::cerr << e.what() << '\n';
                        }
                    }
                }
                _wait(10);
            }
        };
 


        std::function<void(void)> listen_loop_MAIN = [&]() -> void {
            while (MASTER_KEY) {
                try {
                    
                    qProcess = make_shared<HTTP_QUERY>();
                    control = make_shared<T>();

                    if (!control->create())
                    {
                        std::range_error("error al crear");
                    }

                    control->setBuffer(BUFFER);
                    control->setPort(PORT);
                    control->setSessions(SESSION);

                    if (!control->on()) {
                        std::range_error("error al lanzar");
                    }
                    add_queue(control);
                }
                catch (const std::exception &e) {
                    std::cerr << e.what() << '\n';
                }
            }
        };

        std::thread _sender(listen_loop_MAIN);
        std::thread _response(hilo_envia);

        std::thread procesador_base(Worker_maestro->getWorker(macaco, victor, victoria));

        _sender.join();
        _response.join();

        procesador_base.join();
    }

    template <class T>
    void Neody<T>::add_queue(std::shared_ptr<T> &base)
    {
        switch (next_register)
        {
        case 0:
            worker_one.push_back(base);
            {
                condition_one.notify_all();
                macaco.unlock();
            }
            // next_register++;
            break;
        default:
            break;
        }
    }

    template <class T>
    int Neody<T>::setPort(uint16_t _port) noexcept
    {
        if (_port >= 100)
        {
            PORT = std::move(_port);
        }
        else
        {
            return -1;
        }
        return 0;
    }

    typedef Neody<Server> Router;
}

#endif // NEODIMIO_HPP