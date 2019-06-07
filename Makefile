CC = gcc
CFLAGS = -W -Wall -lstdc++
TARGET = hs hc
#OBJECTS = server.cpp client.cpp

all : $(TARGET)

hs : server.cpp
	$(CC) $(CFLAGS) -o $@ $<

hc : client.cpp
	$(CC) $(CFLAGS) -o $@ $<

#($TARGET) : $(OBJECTS)
#	$(CC) $(CFLAGS) -o $(TARGET) $@ $^

clean :
	rm hs hc
