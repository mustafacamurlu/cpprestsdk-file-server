FROM mstfcmrl/cpprestsdk:latest as build
WORKDIR /src
COPY . /src/
RUN mkdir build
WORKDIR /src/build
RUN cmake .. && make

FROM mstfcmrl/alpine-run-base:latest as runtime
RUN mkdir /file_server
ENV LD_LIBRARY_PATH="/file_server"
COPY --from=build /usr/local/lib/libcpprest.so.2.10 /file_server/libcpprest.so.2.10
COPY --from=build /src/build/liblibcontroller.so.1 /file_server/liblibcontroller.so.1
COPY --from=build /src/build/examples/file_server/file_server /file_server/file_server
CMD [ "./file_server/file_server" ]
