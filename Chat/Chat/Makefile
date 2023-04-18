SRC = Chat.cpp
TARGET = Chat
LIBSHA1 = sha1
LIBMSG = message
LIBUSR = User
LIBCHAT = chat
LIBSOCKET = socket
PREFIX = /usr/local/bin

chat:	$(SRC) libsha1 libmessage libuser libchat_client libchat_server libsocket
	g++ -o $(TARGET) $(SRC) -L. -lsha1 -lchat_message -lchat_user -lchat_client -lchat_server -lsocket -std=c++17

libsha1:	$(LIBSHA1).cpp $(LIBSHA1).h
	g++ -o $(LIBSHA1).o $(LIBSHA1).cpp -c
	ar rc lib$(LIBSHA1).a $(LIBSHA1).o

libmessage:	$(LIBMSG).cpp $(LIBMSG).h
	g++ -o $(LIBMSG).o $(LIBMSG).cpp -c
	ar rc libchat_$(LIBMSG).a $(LIBMSG).o

libuser:	$(LIBUSR).cpp $(LIBUSR).h
	g++ -o $(LIBUSR).o $(LIBUSR).cpp -c
	ar rc libchat_$(LIBUSR).a $(LIBUSR).o
	
libsocket:	$(LIBSOCKET).cpp $(LIBSOCKET).h
	g++ -o $(LIBSOCKET).o $(LIBSOCKET).cpp -c
	ar rc lib$(LIBSOCKET).a $(LIBSOCKET).o

libchat_client:	$(LIBCHAT)_client.cpp $(LIBCHAT)_client.h
	g++ -o $(LIBCHAT)_client.o $(LIBCHAT)_client.cpp -c -std=c++17
	ar rc lib$(LIBCHAT)_client.a $(LIBCHAT)_client.o
	
libchat_server:	$(LIBCHAT)_server.cpp $(LIBCHAT)_server.h
	g++ -o $(LIBCHAT)_server.o $(LIBCHAT)_server.cpp -c -std=c++17
	ar rc lib$(LIBCHAT)_server.a $(LIBCHAT)_server.o

clean:
	rm *.o *.a

install:
	install $(TARGET) $(PREFIX)

uninstall:
	rm -rf $(PREFIX)/$(TARGET)
