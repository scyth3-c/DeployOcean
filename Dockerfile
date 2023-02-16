FROM debian:bullseye

RUN apt-get update
RUN apt-get install libstdc++-10-dev -y && \
     apt-get install -y g++-10 && \
     apt-get install g++-10-multilib -y && \
     apt-get install -y make 


RUN mkdir neody
WORKDIR /neody
COPY . ./

RUN ln -sf /usr/bin/g++-10 /usr/bin/g++
RUN make
RUN chmod 777 binary

EXPOSE 3000
# RUN adduser neody
# RUN chown neody ./binary 
# USER neody

CMD ./binary