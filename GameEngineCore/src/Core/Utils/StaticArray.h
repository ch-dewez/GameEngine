#pragma once
// an dynamic array which does not reallocate element when growing but does not guarrentee memory locality
// | | | | |                | | | | | | | | | | | | | | |
//          no more space
//          allocating more but somewhere else
#include <functional>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include "Core/Log/Log.h"
#include <vector>

namespace Engine {
namespace Utils {

template<typename BaseOfAll>
class StaticArrayRegistry;

template<typename T>
class StaticArray {
public:
    StaticArray() = default;
    StaticArray(int size)
    {
        resize(size);
    };
    virtual ~StaticArray()
    {
        for (int i=0;i<m_count;i++){
            T* value = m_values[i];
            value->~T();
        }
        for (T* p : m_pools){
            LogDebug("calling delete");
            delete[] p;
        }
    };

    int size()
    {
        return m_count;
    };
    int capicity()
    {
        return m_values.size();
    };

    void resize(int size)
    {
        reserve(size);
        m_count = size;
    };
    // reserve ignore next pool size it just create one pool that is the size of capacity - size (next pool size is used with add())
    void reserve(int size)
    {
        if (size <= capicity()){
            return;
        }

        int sizeToAllocate = size - capicity();
        // Allocate memory
        T* pool = (T*) new char[m_nextPoolSize * sizeof(T)];
        m_pools.push_back(pool);

        for (int i=0;i<sizeToAllocate;i++){
            m_values.push_back(&pool[i]);
        }

        m_nextPoolSize = capicity() * 2;
    };

    T& operator[](int index){
        Assert(
            [&] () {
                for (int i=0;i<m_frees.size();i++){
                    if (m_frees[i] == index){
                        return false;
                    }
                }
                return true;
            } ()
            , "Trying to get index but index is freed");
        Assert(index <= m_count, "Index out of range");
        return *m_values[index];
    };

    T& add(T& element){
        if (m_frees.size() > 0) {
            int back = m_frees.back();
            *m_values[back] = element;
            m_frees.pop_back();
            return *m_values[back];
        }

        if (capicity() - size() != 0){
            *m_values[m_count] = element;
        }

        if (capicity() - size() == 0){
            reserve(m_nextPoolSize);
            *m_values[m_count] = element;
        }

        m_count++;
        return *m_values[m_count - 1];
    };

    // C allow for creating derived classes
    template<typename C = T, class... Args>
    T& add_emplace(Args... args){
        if (m_frees.size() > 0) {
            int back = m_frees.back();
            new (m_values[back]) C(args...);
            m_frees.pop_back();
            return *m_values[back];
        }

        if (capicity() - size() != 0){
            new (m_values[m_count]) C(args...);
        }

        if (capicity() - size() == 0){
            reserve(m_nextPoolSize);
            new (m_values[m_count]) C(args...);
        }

        m_count++;
        return *m_values[m_count - 1];
    };
    void remove(int index){
        m_values[index]->~T();
        m_frees.push_back(index);
    }; // does not reallocate; index is add in a list which is is then filled with add()
    void filter(std::function<bool(T&)> condition){
        for (int i=0;i<size();i++){
            if (condition(*m_values[i])) {
                remove(i);
            }
        }
    };
    void filterFirst(std::function<bool(T&)> condition){
        for (int i=0;i<size();i++){
            if (condition(*m_values[i])) {
                remove(i);
                return;
            }
        }
    };

    std::vector<T&> getFilteredVector(std::function<bool(T&)> condition){
        std::vector<T&> result;
        for (T* value : m_values){
            if (condition(*value)) {
                result.push_back(value);
            }
        }
    };
    std::optional<T*> getFirstMatch(std::function<bool(T&)> condition){
        for (T* value : m_values){
            if (condition(*value)) {
                return value;
            }
        }
    };

private:
    std::vector<T*> m_values; // vector that countains pointer to each element (even if not used -> .size() = capicity != size)
    std::vector<T*> m_pools; // vector that countatins pointer to the start of each pools
    std::vector<int> m_frees; // free inside of count

    int m_nextPoolSize = 5;
    int m_count=0; // size of static array != capacity
public:
    class Iterator {
    public:
        Iterator(StaticArray* host, int index)
        : m_host(host), m_index(index) {};

        void reset(){ m_index = 0; }
        bool operator!=(const Iterator& other) const {return m_index != other.m_index; }
        const Iterator& operator++() {m_index++;return *this;}
        T& operator*() const {return (*m_host)[m_index];}
    private:
        StaticArray* m_host;
        int m_index;
    };

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, m_count);
    }
};

// contain one static array per type
template<typename BaseOfAll>
class StaticArrayRegistry {
public:
    ~StaticArrayRegistry(){
        for (auto& array : m_arrays){
            delete array.second;
        }
    }

    template<typename T>
    StaticArray<T>& getArray(){
        static_assert(std::is_base_of<BaseOfAll,T>::value, "T must derive from Interface");
        const std::type_index typeId = std::type_index(typeid(T));

        auto it = m_arrays.find(typeId);
        if (it == m_arrays.end()) {
            m_arrays[typeId] = (StaticArray<BaseOfAll>*)new StaticArray<T>();
            
            return (StaticArray<T>&)*m_arrays[typeId];
        }

        return *(StaticArray<T>*)it->second;
    };

    std::unordered_map<std::type_index, StaticArray<BaseOfAll>*>& getAllArrays() {return m_arrays;};


    template<typename Base>
    std::vector<Base*> getAllElementOfType(){
        std::vector<Base*> result;
        std::vector<StaticArray<Base>*> allArrays = getAllArraysOfType<Base>();

        for (StaticArray<Base>* array : allArrays){
            for (Base& element : *array){
                result.push_back(&element);
            }
        }
        return result;
    }

    template<typename Base>
    std::vector<StaticArray<Base>*> getAllArraysOfType() {
        std::vector<StaticArray<Base>*> result;
        std::type_index baseTypeId = std::type_index(typeid(Base));
        
        for (auto& pair : m_arrays) {
            auto name = pair.first.name();
            if (baseTypeId == pair.first || isBaseOf<Base>(pair.second)) {
                result.push_back((StaticArray<Base>*)pair.second);
            }
        }
        return result;
    }

    template<typename T>
    bool isBaseOf(StaticArray<BaseOfAll>* base){
        if (base->size() <= 0){
            return false;
        }
        BaseOfAll* pt = &((*base)[0]);
        if (dynamic_cast<T*>(pt)){
            return true;
        }
        return false;
    }

private:
    std::unordered_map<std::type_index, StaticArray<BaseOfAll>*> m_arrays;
};

}
}
