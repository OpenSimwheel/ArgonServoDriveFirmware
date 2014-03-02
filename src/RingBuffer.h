/*
 * RingBuffer.h
 *
 *  Created on: Jan 2, 2012
 *      Author: tero
 *
 *  Modified on: Mar 1, 2014
 *  	Author: bberger
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "types.h"
#include "stdlib.h"
#include "stm32f2xx.h"

template <typename T> class RingBuffer
{
public:
    RingBuffer( size_t bufferSize ) {
        allocate(bufferSize);
    }

    RingBuffer() {
        buf=NULL;
    }

    virtual ~RingBuffer(){
        free(buf);
    }

    //same as constructor with param
    void allocate( size_t bufferSize ){
        buf=(T*)malloc(sizeof(T) * bufferSize);
        tail=head=0;
        size = bufferSize;
        bytesAvail=0;
    }

    //insert byte in FIFO
    //return false if buffer full
    bool put(T data) {
        size_t hd=head;
        hd++;

        if(hd >= size)
            hd=0;

        if(bytesAvail >= size)
            return false;//full

        buf[head]=data;
        head=hd;
        bytesAvail++;
        return true;
    }

    //sets buffer full =true if not enough space
    void put( T data, bool &bufferFull ){
        bufferFull = (put(data) == false);
    }

    //get byte from FIFO
    //sets error to true if buffer is empty
    T get( bool &empty ){
        T ret;
        size_t tl=tail;
        if(bytesAvail<=0)
        {
            //nodata
            empty=true;
            return 0;
        }
        bytesAvail--;
        ret=buf[tl];
        tl++;
        if(tl>=size)tl=0;
        tail=tl;
        return ret;
}

    T get() {
        bool trash;
        return get(trash);
    }

    //get oldest byte without removing it from FIFO
    T peek( bool &empty ){
        if(bytesAvail<=0)
        {
            //nodata
            empty=true;
            return 0;
        }

        return buf[tail];
    }

    T peek (size_t index) {
        if (bytesAvail <= 0)
            return 0;

        return buf[index];
    }

    T& operator[](size_t index) {
        return buf[index];
    }

    //empty all data
    void clear(){
        tail=head=bytesAvail=0;
    }

    //number of bytes to read in FIFO
    size_t getOccupied() const{
        return bytesAvail;
    }
    //number of bytes of free space in FIFO
    size_t getFree() const{
        return (size-bytesAvail);
    }

    bool test() {
        size_t size=33;
        RingBuffer<T> b;
        b.allocate(size);
        T i;
        bool fail=true;

        if(b.getFree()!=size)fail=true;
        if(b.getOccupied()!=0)fail=true;

        for(i=0;i<size;i++)
            b.put(i,fail);

        if(b.getFree()!=0)fail=true;
        if(b.getOccupied()!=size)fail=true;

        for(i=0;i<size;i++)
            if(b.get(fail)!=i) fail=true;

        if(b.getFree()!=size)fail=true;
        if(b.getOccupied()!=0)fail=true;

        for(i=0;i<size;i++)
            b.put(i,fail);

        if(b.getFree()!=0)fail=true;
        if(b.getOccupied()!=size)fail=true;

        for(i=0;i<size;i++)
            if(b.get(fail)!=i) fail=true;

        b.get(fail);//should fail

        return fail;
    }
private:

    T *buf;
    size_t size, tail, head, bytesAvail;
};

#endif /* RINGBUFFER_H_ */
