FROM mstfcmrl/cpprestsdk:latest as build
WORKDIR /src
COPY CMakeLists.txt *.cpp *.hpp ./
RUN mkdir build
WORKDIR /src/build
RUN cmake .. && make

FROM mstfcmrl/alpine-run-base:latest as runtime
RUN mkdir /file_server
ENV LD_LIBRARY_PATH="/file_server"
ENV FILE_SERVER_BASE="./"
COPY --from=build /usr/local/lib/libcpprest.so.2.10 /file_server/libcpprest.so.2.10
COPY --from=build /src/build/FileServer /file_server/FileServer
CMD [ "./file_server/FileServer" ]
