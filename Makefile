CC=gcc
FLAGS = -I ./Ntyco/core/ -L ./Ntyco/ -lntyco

TARGET = kvstore
SRCS=kvstore_mp.c epoll_entry.c kvstore.c kvstore_array.c kvstore_rbtree.c kvstore_hash.c  ntyco_entry.c

SUBDIR=./Ntyco

all: $(SUBDIR) $(TARGET)

$(SUBDIR):
	make -C $@

ECHO:
	@echo $(SUBDIR)

$(TARGET):$(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(FLAGS)

clean: 
	rm -r $(TARGET)
