#include <iostream>
#include <stdio.h>
#include "neody/neocore.h"

using  neo::Router;

int main() {

    Router router;
    uint16_t puerto = 3000;


    router.setPort(puerto);
    std::cout << "servidor corriendo en el puerto: "<< puerto << std::endl;


    router.get("verify",{ [&](Query &http) {

        http.html("<h1>macaco!</h1>");
    
    }});


    router.get("/",{ [&](Query &http) {
        http.readFile("./index.html", "text/html");
    }});


    router.listen();
    
    return 0;
}