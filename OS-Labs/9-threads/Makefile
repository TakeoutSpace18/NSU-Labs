CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: threads_1 threads_2 threads_3 threads_4 threads_5

threads_1: 1_1_threads.c
	${CC} ${CFLAGS} -I${INCLUDE_DIR} 1_1_threads.c ${LIBS} -o threads_1

threads_2: 1_2_threads.c
	${CC} ${CFLAGS} -I${INCLUDE_DIR} 1_2_threads.c ${LIBS} -o threads_2

threads_3: 1_3_threads.c
	${CC} ${CFLAGS} -I${INCLUDE_DIR} 1_3_threads.c ${LIBS} -o threads_3

threads_4: 1_4_threads.c
	${CC} ${CFLAGS} -I${INCLUDE_DIR} 1_4_threads.c ${LIBS} -o threads_4

threads_5: 1_5_threads.c
	${CC} ${CFLAGS} -I${INCLUDE_DIR} 1_5_threads.c ${LIBS} -o threads_5

clean:
	${RM} -f *.o ${TARGET}
