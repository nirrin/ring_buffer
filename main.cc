#include <cstdlib>
#include <ctime>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <map>
#include <limits>
#include <iomanip>
#include <cmath>
#include <cstring>
#include "market_data.h"

template <typename T>
class RingBuffer {
public:

    RingBuffer(const std::size_t size_) : size(size_), start(0), end(0), s_msb(0), e_msb(0), elems(NULL) {       
        init();
    }

    RingBuffer(const std::size_t size_, const T& elem) : size(size_), start(0), end(0), s_msb(0), e_msb(0), elems(NULL) {      
        init();
        write(elem);
    }
    
     RingBuffer(const std::size_t size_, const T& elem, const std::size_t elem_size) : size(size_), start(0), end(0), s_msb(0), e_msb(0), elems(NULL) {      
        init();
        write(elem, elem_size);
    }

    ~RingBuffer() {      
        if (elems) {
            delete[] elems;
        }
    }

    inline bool is_full() const {
        return ((end == start) && (e_msb != s_msb));
    }

    inline bool is_empty() const {
        return ((end == start) && (e_msb == s_msb));
    }

    inline void write(const T& elem) {
        elems[end] = elem;
        post_write();
    }

    inline void write(const T& elem, const std::size_t size_) {
        memmove((void*) &(elems[end]), (void*) &elem, size_);
        post_write();
    }

    inline T& read() {
        const size_t index = start;
        incr(start, s_msb);
        return elems[index];
    }

    inline T& peek() const {
        return elems[start];
    }

    friend inline std::ostream& operator<<(std::ostream& os, const RingBuffer<T>& buffer) {
        os << "{ \"full\": \"" << std::boolalpha << buffer.is_full() <<
                "\", \"empty\": \"" << buffer.is_empty() << "\"" <<
                ", \"size\": " << buffer.size <<
                ", \"start\": " << buffer.start <<
                ", \"end\": " << buffer.end <<
                ", \"start_msb\": " << buffer.s_msb <<
                ", \"end_msb\": " << buffer.e_msb <<
                " }";
        return os;
    }

private:
    std::size_t size; /* maximum number of elements */
    std::size_t start; /* index of oldest element */
    std::size_t end; /* index at which to write new element */
    short int s_msb;
    short int e_msb;  
    T* elems;

    inline void incr(std::size_t& p, short int& msb) const {
        if (++p == size) {
            msb ^= 1;
            p = 0;
        }
    }

    inline void post_write() {
        if (is_full()) { /* full, overwrite moves start pointer */
            incr(start, s_msb);
        }
        incr(end, e_msb);
    }

    inline void init() {
        elems = new T[size];
    }
};

class Prices : public std::unordered_map<uint32_t, RingBuffer<CombinedSortedBook>*>
{
    public:

    Prices(const size_t size_) : size(size_) {
    }
    
    ~Prices() {
        for (Prices::iterator i = begin(); i != end(); i++) {
            delete i->second;
        }
        clear();         
    }

    inline void write(const CombinedSortedBook & book) {
        const uint32_t hash = book.hash();
        Prices::iterator i = find(hash);
        if (i != end()) {
            i->second->write(book, book.TransmitSize());
        } else {
            insert({hash, new RingBuffer<CombinedSortedBook>(size, book, book.TransmitSize())});
        }
    }

    friend inline std::ostream& operator<<(std::ostream& os, Prices & prices) {
        for (Prices::iterator i = prices.begin(); i != prices.end(); i++) {
            if (!i->second->is_empty()) {
                CombinedSortedBook book = i->second->peek();
                os << i->first << "\t" << book.header.client_instrument_id << "\t" << (int) book.header.ecn << std::endl;
            }
        }
        return os;
    }

    private:
    std::size_t size;
};

int main() {
    std::srand(std::time(0));
    Prices* prices = new Prices(100);
    
    for (size_t i = 0; i < 1000000; i++) {
        CombinedSortedBook book;
        book.header.client_instrument_id = (ClientInstrumentId) (41.0 * std::rand() / RAND_MAX); //(ClientInstrumentId) (i % 40); //(ClientInstrumentId) (40.0 * std::rand() / RAND_MAX);
        book.header.ecn = (ExchangeId) (29.0 * std::rand() / RAND_MAX); //(ExchangeId) (i % 30); //(ExchangeId) (30.0 * std::rand() / RAND_MAX);
        book.header.bid_num_tiers = 10;
        book.header.ask_num_tiers = 10;
        prices->write(book);
    }
    delete prices;   
}
