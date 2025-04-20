CC = gcc
FLAGS = -I ./Ntyco/core/ -L ./Ntyco/ -lntyco -lpthread -luring -ldl -lrdkafka
SRCS = kvstore.c ntyco.c proactor.c kvs_array.c kvs_rbtree.c kvs_hash.c
TARGET = kvstore
SUBDIR = ./Ntyco/

OBJS = $(SRCS:.c=.o)

all: $(SUBDIR) $(TARGET) 

$(SUBDIR): ECHO
	make -C $@

ECHO:
	@echo $(SUBDIR)

$(TARGET): $(OBJS) 
	$(CC) -o $@ $^ $(FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@

clean: 
	rm -rf $(OBJS) $(TARGET) 
