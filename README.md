## Bug Zapper

Implement a QT GUI based Application for posting/discussing/prioritizing bugs during 
development within a team. Planning to have either local or server based as option to 
co-work offline or online. likely FTP server, login functionality, and other useful 
features that i will figure out on the journey. 

# Compile and Run Instructions

```sh
     brew install qt6
```
Server:
```sh
    cd [root directory name]/server
    ./build.sh --run server
```
Client(while Server is running):
```sh
    cd [root directory name]/client
    ./build.sh --run client
```

to cleanup build files in either:
```sh
    ./build.sh --clean-all
```

# Dependencies:

QT6


