# libcontroller
Controller like support on top of cpprestsdk

# To build
chmod +x build.sh

./build.sh

It'll create *file_server:latest* in your docker image repository

# To run
docker run --rm -d -p 8081:8081 file_server
