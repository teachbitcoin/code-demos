# g++ -std=c++11 main.cpp database.cpp wallet.cpp ../src/database.cpp ../src/wallet.cpp -o main -lsqlite3 $(pkg-config --cflags libbitcoin --libs libbitcoin)

CPPFLAGS= -std=c++11 $(shell pkg-config --cflags libbitcoin-client)
LDFLAGS= -std=c++11
LDLIBS=$(shell pkg-config --libs libbitcoin-client) -lsqlite3
RM=rm -f

SRCS=main.cpp wallet.cpp database.cpp ../src/wallet.cpp ../src/database.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	g++ $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

main.o: main.cpp
	g++ $(CPPFLAGS) -c main.cpp

wallet.o: ../include/wallet.hpp wallet.cpp
	g++ $(CPPFLAGS) -c wallet.cpp

database.o: ../include/database.hpp database.cpp
	g++ $(CPPFLAGS) -c database.cpp

../src/wallet.o: ../include/wallet.hpp ../src/wallet.cpp ../include/database.hpp
	g++ $(CPPFLAGS) -c ../src/wallet.cpp -o ../src/wallet.o

../src/database.o: ../include/database.hpp ../src/database.cpp
	g++ $(CPPFLAGS) -c ../src/database.cpp -o ../src/database.o

clean:
	$(RM) $(OBJS)
