CC=g++
CFLAGS=-c -Wall -std=c++11
INCLUDES= \
-I/media/root/emmclinux/DothPlace/DothProject/DI2C \
-I/media/root/emmclinux/DothPlace/DothProject/DTL 


SOURCES= PCA9685.cpp 
 
OBJECTS=$(SOURCES:.cpp=.o)
STATICLIB=dft
MOVE=qwe


all: $(SOURCES) $(STATICLIB) $(MOVE)
	
$(STATICLIB): $(OBJECTS) 
	ar rvs PCA9685Static.a $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $@

$(MOVE):
	cp PCA9685Static.a /media/root/emmclinux/DothPlace/DothProject/Linked/PCA9685Static.a

clean:
	rm PCA9685Static.a *.o
	

