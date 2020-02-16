#ifndef THREADSAFE_UNORDERED_MAP_H_
#define THREADSAFE_UNORDERED_MAP_H_

#include <algorithm> //find_if
#include <vector> //vector
#include <map> //map
#include <list> //list
#include <memory> //unique_ptr
#include <mutex> //lock_guard
#include <boost/thread/shared_mutex.hpp> // shared_mutex

namespace ws{

namespace detail{

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class Threadsafe_unordered_map{
    private:
        class buket_type{
            friend class Threadsafe_unordered_map;
            private:
                using buket_value = std::pair<Key, Value>;
                using buket_data = std::list<buket_value>;
                using buket_iterator = typename buket_data::iterator;
                
                buket_data data;
                mutable boost::shared_mutex mutex;

                buket_iterator
                find_entry(const Key& key) { //TODO const有点意思 再看看
                    return std::find_if(
                        data.begin(), data.end(),
                        [&](const buket_value& para){
                            return para.first == key;
                        }
                    );
                }

            public:
                Value value_of(const Key& key, const Value& default_value) const {
                    boost::shared_lock<boost::shared_mutex> lk(mutex);
                    const buket_iterator value_iterator = find_entry(key);
                    return (value_iterator == data.end()) ? 
                        default_value : value_iterator.second;
                }

                void add_or_update(const Key& key, const Value& value){
                    std::lock_guard<boost::shared_mutex> guard(mutex);
                    const buket_iterator value_iterator = find_entry(key);
                    if(value_iterator == data.end()){
                        data.push_back(buket_value(key, value));
                    }else{
                        value_iterator->second = value;
                    }
                }

                void remove(const Key& key){
                    std::unique_lock<boost::shared_mutex> guard(mutex);
                    const buket_iterator value_iterator = find_entry(key);
                    if(value_iterator != data.end()){
                        data.erase(value_iterator);
                    }
                }
        };

        std::vector<std::unique_ptr<buket_type>> bukets;
        Hash hasher;

        buket_type& get_buket(const Key& key) const {
            const std::size_t index = hasher(key) % bukets.size();
            return *bukets[index]; 
        }

    public:
        using key_type = Key;
        using value_type = Value;
        using hash_type = Hash;

        explicit Threadsafe_unordered_map(unsigned num_bukets = 23, const Hash& hasher_ = Hash())
            : hasher(hasher_), bukets(num_bukets){
                for(size_t i = 0; i < num_bukets; i++){
                    bukets[i].reset(new buket_type);
                }
            }

        Threadsafe_unordered_map(const Threadsafe_unordered_map&) = delete;
        Threadsafe_unordered_map& operator=(
            const Threadsafe_unordered_map&
        ) = delete; 

        value_type value_of(const key_type& key, const value_type& value = value_type()) const {
            return get_buket(key).value_of(key, value);
        }

        void add_or_update(const key_type& key, const value_type& value){
            return get_buket(key).add_or_update(key, value);
        }

        void remove(const key_type& key){
            return get_buket(key).remove(key);
        }

        std::map<Key, Value> 
        get_standard_map() const;
};

//使用读写锁时需使用unique_lock 报错是使用了一个删除的功能
template<typename Key, typename Value, typename Hash>
std::map<Key, Value>
Threadsafe_unordered_map<Key, Value, Hash>::get_standard_map() const {
    std::map<key_type, value_type> instance;
    std::vector<std::unique_lock<boost::shared_mutex>> lk;
    for(size_t i = 0; i < bukets.size(); i++){
        lk.emplace_back(std::unique_lock<boost::shared_mutex>(bukets[i]->mutex));
    }
    for(size_t i = 0; i < bukets.size(); i++){
/*         std::for_each(bukets[i]->data.begin(), bukets[i]->data.end(),
            [&](const typename buket_type::buket_iterator& bi){
                instance.insert(*bi);
            }); */ //找不出有什么问题
        for(typename buket_type::buket_iterator bt = bukets[i]->data.begin();
            bt != bukets[i]->data.end(); ++bt){
                instance.insert(*bt);
            }
    }
    return std::move(instance);
}

}

}

#endif //THREADSAFE_UNORDERED_MAP_H_