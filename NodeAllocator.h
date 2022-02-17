//
// Created by robin on 14.02.22.
//

#ifndef READING_NODEALLOCATOR_H
#define READING_NODEALLOCATOR_H

#include <cstdint>
#include "Node.h"
#include <memory>
#include <list>


template<size_t board_size>
class Node;

template<size_t board_size>
constexpr size_t get_num_entries(size_t num_bytes) {
    return num_bytes / sizeof(Node<board_size>);
}


template<size_t board_size>
struct Page {

    static constexpr size_t MAX_ALLOC = get_num_entries<board_size>(400000);
    Node<board_size> *entries;

    Page() {
        entries = (Node<board_size> *) malloc(sizeof(Node<board_size>) * MAX_ALLOC);
        for (auto i = 0; i < MAX_ALLOC; ++i) {
            entries[i] = Node<board_size>{};
        }
    }

    ~Page() {
        free(entries);
    }
};


template<std::size_t board_size>
class NodeAllocator {
public:
    using page_ptr = std::unique_ptr<Page<board_size>>;
    using Type = Node<board_size>;
    size_t offset;
    std::vector<page_ptr> pages;
    size_t num_allocations{0};
public:

    void init() {
        if (pages.empty()) {
            pages.emplace_back(std::make_unique<Page<board_size>>());
            offset = 0;
        }
    }

    inline Node<board_size> *allocate() {
        if (offset >= Page<board_size>::MAX_ALLOC) {
            //need to add a new page
            pages.emplace_back(std::make_unique<Page<board_size>>());
            offset = 0;
        }
        Page<board_size> *current = pages.back().get();
        num_allocations++;
        return current->entries + offset++;
    }

    inline Node<board_size> *allocate_children(size_t num_children) {
        //returns a pointer to the first child node
        //if the array of children does not fit in the current page
        //they need to be placed in the next page !!!
        if (offset + num_children >= Page<board_size>::MAX_ALLOC) {
            pages.emplace_back(std::make_unique<Page<board_size>>());
            offset = 0;
        }
        Page<board_size> *current = pages.back().get();
        Node<board_size> *c_node = current->entries + offset;
        offset += num_children;
        num_allocations += num_children;
        return c_node;
    }


    inline void free(Type *ptr) {
        //do something or not
        //decrement offset for now

    }

    void release() {
        //forcing deallocation of the pages
        pages.clear();
        offset = 0;
    }

    void reset() {
        //release();
        pages.clear();
        pages.emplace_back(std::make_unique<Page<board_size>>());
        offset = 0;
    }

    size_t get_num_allocations() {
        return num_allocations;
    }

    size_t get_num_pages() {
        return pages.size();
    }

    inline static NodeAllocator &get_instance() {
        static NodeAllocator<board_size> alloc;
        return alloc;
    }


};


#endif //READING_NODEALLOCATOR_H
