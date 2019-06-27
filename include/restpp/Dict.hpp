//
// copy from https://github.com/EttusResearch/uhd/blob/master/host/include/uhd/types/Dict.ipp
//

#ifndef RESTPP_Dict_HPP
#define RESTPP_Dict_HPP

#include <list>
#include <vector>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <restpp/Exception.hpp>

namespace restpp {

/*!
 * A template Dictionary class with a python-like interface.
 */
    template<typename Key, typename Val>
    class Dict {
    public:
        /*!
         * Create a new empty Dictionary.
         */
        Dict();

        /*!
         * Input iterator constructor:
         * Makes boost::assign::map_list_of work.
         * \param first the begin iterator
         * \param last the end iterator
         */
        template<typename InputIterator>
        Dict(InputIterator first, InputIterator last);

        /*!
         * Get the number of elements in this Dict.
         * \return the number of elements
         */
        std::size_t size() const;

        /*!
         * Get a list of the keys in this Dict.
         * Key order depends on insertion precedence.
         * \return vector of keys
         */
        std::vector<Key> keys() const;

        /*!
         * Get a list of the values in this Dict.
         * Value order depends on insertion precedence.
         * \return vector of values
         */
        std::vector<Val> vals() const;

        /*!
         * Does the Dictionary contain this key?
         * \param key the key to look for
         * \return true if found
         */
        bool has_key(const Key &key) const;

        /*!
         * Get a value in the Dict or default.
         * \param key the key to look for
         * \param other use if key not found
         * \return the value or default
         */
        const Val &get(const Key &key, const Val &other) const;

        /*!
         * Get a value in the Dict or throw.
         * \param key the key to look for
         * \return the value or default
         */
        const Val &get(const Key &key) const;

        /*!
         * Set a value in the Dict at the key.
         * \param key the key to set at
         * \param val the value to set
         */
        void set(const Key &key, const Val &val);

        /*!
         * Get a value for the given key if it exists.
         * If the key is not found throw an error.
         * \param key the key to look for
         * \return the value at the key
         * \throw an exception when not found
         */
        const Val &operator[](const Key &key) const;

        /*!
         * Set a value for the given key, however, in reality
         * it really returns a reference which can be assigned to.
         * \param key the key to set to
         * \return a reference to the value
         */
        Val &operator[](const Key &key);

        /*!
         * Equals operator for the Dict type
         * \param other the Dict being compared to this
         * \return whether or not the two Dict's are equal
         */
        bool operator==(const Dict<Key, Val> &other) const;

        /*!
         * Not equal operator for the Dict type
         * \param other the Dict being compared to this
         * \return whether or not the two Dict's are not equal
         */
        bool operator!=(const Dict<Key, Val> &other) const;

        /*!
         * Pop an item out of the Dictionary.
         * \param key the item key
         * \return the value of the item
         * \throw an exception when not found
         */
        Val pop(const Key &key);

        /*! Update this Dictionary with values from another.
         *
         * Basically, this copies all the key/value pairs from \p new_Dict
         * into this Dict. When the key is already present in the current
         * Dict, it either overwrites the current value (if \p fail_on_conflict
         * is false) or it throws (if \p fail_on_conflict is true *and* the
         * values differ).
         *
         * With the exception of \p fail_on_conflict, this behaves analogously
         * to Python's Dict.update() method.
         *
         * \param new_Dict The arguments to copy.
         * \param fail_on_conflict If true, throws.
         * \throws restpp::ValueError
         */
        void update(const Dict<Key, Val> &new_Dict, bool fail_on_conflict = true);

    private:
        typedef std::pair<Key, Val> pair_t;
        std::list<pair_t> _map; // private container
    };

} // namespace restpp



namespace restpp {

    namespace /*anon*/{
        template<typename Key, typename Val>
        struct key_not_found : restpp::KeyError {
            explicit key_not_found(const Key &key) : restpp::KeyError(
                    str(boost::format(
                            "key \"%s\" not found in Dict(%s, %s)"
                        ) % boost::lexical_cast<std::string>(key)
                        % typeid(Key).name() % typeid(Val).name()
                    )
            ) {
                /* NOP */
            }
        };
    } // namespace /*anon*/

    template<typename Key, typename Val>
    Dict<Key, Val>::Dict() {
        /* NOP */
    }

    template<typename Key, typename Val>
    template<typename InputIterator>
    Dict<Key, Val>::Dict(InputIterator first, InputIterator last):
            _map(first, last) {
        /* NOP */
    }

    template<typename Key, typename Val>
    std::size_t Dict<Key, Val>::size() const {
        return _map.size();
    }

    template<typename Key, typename Val>
    std::vector<Key> Dict<Key, Val>::keys() const {
        std::vector<Key> keys;
        for (const pair_t &p: _map) {
            keys.push_back(p.first);
        }
        return keys;
    }

    template<typename Key, typename Val>
    std::vector<Val> Dict<Key, Val>::vals() const {
        std::vector<Val> vals;
        for (const pair_t &p: _map) {
            vals.push_back(p.second);
        }
        return vals;
    }

    template<typename Key, typename Val>
    bool Dict<Key, Val>::has_key(const Key &key) const {
        for (const pair_t &p: _map) {
            if (p.first == key) return true;
        }
        return false;
    }

    template<typename Key, typename Val>
    const Val &Dict<Key, Val>::get(const Key &key, const Val &other) const {
        for (const pair_t &p: _map) {
            if (p.first == key) return p.second;
        }
        return other;
    }

    template<typename Key, typename Val>
    const Val &Dict<Key, Val>::get(const Key &key) const {
        for (const pair_t &p: _map) {
            if (p.first == key) return p.second;
        }
        throw key_not_found<Key, Val>(key);
    }

    template<typename Key, typename Val>
    void Dict<Key, Val>::set(const Key &key, const Val &val) {
        (*this)[key] = val;
    }

    template<typename Key, typename Val>
    const Val &Dict<Key, Val>::operator[](const Key &key) const {
        for (const pair_t &p: _map) {
            if (p.first == key) return p.second;
        }
        throw key_not_found<Key, Val>(key);
    }

    template<typename Key, typename Val>
    Val &Dict<Key, Val>::operator[](const Key &key) {
        for (pair_t &p: _map) {
            if (p.first == key) return p.second;
        }
        _map.push_back(std::make_pair(key, Val()));
        return _map.back().second;
    }

    template<typename Key, typename Val>
    bool Dict<Key, Val>::operator==(const Dict<Key, Val> &other) const {
        if (this->size() != other.size()) {
            return false;
        }
        for (const pair_t &p: _map) {
            if (not(other.has_key(p.first) and other.get(p.first) == p.second)) {
                return false;
            }
        }
        return true;
    }

    template<typename Key, typename Val>
    bool Dict<Key, Val>::operator!=(const Dict<Key, Val> &other) const {
        return not(*this == other);
    }

    template<typename Key, typename Val>
    Val Dict<Key, Val>::pop(const Key &key) {
        typename std::list<pair_t>::iterator it;
        for (it = _map.begin(); it != _map.end(); it++) {
            if (it->first == key) {
                Val val = it->second;
                _map.erase(it);
                return val;
            }
        }
        throw key_not_found<Key, Val>(key);
    }

    template<typename Key, typename Val>
    void Dict<Key, Val>::update(const Dict<Key, Val> &new_Dict, bool fail_on_conflict) {
        for (const Key &key: new_Dict.keys()) {
            if (fail_on_conflict and has_key(key) and get(key) != new_Dict[key]) {
                throw restpp::ValueError(str(
                        boost::format("Option merge conflict: %s:%s != %s:%s")
                        % key % get(key) % key % new_Dict[key]
                ));
            }
            set(key, new_Dict[key]);
        }
    }

} //namespace restpp


#endif //RESTPP_Dict_HPP
