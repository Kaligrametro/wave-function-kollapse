#ifndef CUSTOM_OPTIONAL_H
#define CUSTOM_OPTIONAL_H
/*
* My compiler version doesnt have std::optional
* so here is a custom implementation
* It has the same structure as the original in
* order to make it compatible once adapted
*                                         - Cali
*/
namespace std {

    extern constexpr void* nullopt = nullptr;

    template <typename T>
    class optional
    {
    private:
        bool m_exists;
        T    m_value;
    public:

        optional(T value) : 
            m_value(value), m_exists(true) 
        {}
        optional(void* nullopt) : 
            m_exists(false) 
        {}
        optional() : 
            m_exists(false) 
        {}

        bool has_value() const 
        { 
            return m_exists; 
        }

        T value() 
        { 
            return m_value; 
        }

        T value_or(const T& default_value = T()) const 
        { 
            return m_exists ? m_value : default_value; 
        }

        operator bool() const 
        { 
            return m_exists; 
        }

    };
};

#endif
