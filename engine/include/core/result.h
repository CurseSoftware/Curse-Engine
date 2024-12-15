#pragma once
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cstddef>
#include <new>

namespace gravity {
namespace result {

/// @brief Move-only storage class to prevent copying
/// @tparam T Ok type
/// @tparam E Error type
template <typename T, typename E>
class Storage {
public:
    /// @brief Default constructor
    Storage() : current_state(State::Empty) {}

    /// @brief Move constructor
    /// @param other Storage being moved
    Storage(Storage&& other) noexcept {
        current_state = other.current_state;
        if (current_state == State::Ok) {
            new (storage) T(std::move(*reinterpret_cast<T*>(other.storage)));
        } else if (current_state == State::Err) {
            new (storage) E(std::move(*reinterpret_cast<E*>(other.storage)));
        }

        other.reset();
    }

    /// @brief Move assignment operator
    /// @param other The storage that we are moving from
    /// @return The target storage moved to
    Storage& operator=(Storage&& other) noexcept {
        if (this != other) {
            destroy();

            current_state = other.current_state;

            if (current_state == State::Ok) {
                new (storage) T(std::move(*reinterpret_cast<T*>(other.storage)));
            } else if (current_state == State::Err) {
                new (storage) E(std::move(*reinterpret_cast<E*>(other.storage)));
            }

            other.reset();
        }

        return *this;
    }

    /// @brief Deleted copy constructor
    Storage(const Storage&) = delete;

    /// @brief Deleted copy assignment operator
    Storage& operator=(const Storage&) = delete;

    /// @brief Destructor
    ~Storage() {
        destroy();
    }

    /// @brief Store an OK value
    /// @tparam U The ok data type
    /// @param value The ok data to store
    template <typename U>
    void store_ok(U&& value) {
        destroy();
        new (storage) T(std::forward<U>(value));
        current_state = State::Ok;
    }

    /// @brief Store an error value
    /// @tparam U The error type
    /// @param error The error data to store
    template <typename U>
    void store_err(U&& error) {
        destroy();
        new (storage) E(std::forward<U>(error));
        current_state = State::Err;
    }

    /// @brief Return if this is storing an Ok value
    /// @return True if OK false otherwise
    bool is_ok() const {
        return current_state == State::Ok;
    }

    /// @brief Return if this is storing an error value
    /// @return True if Error false otherwise
    bool is_err() const {
        return current_state == State::Err;
    }

    /// @brief Get the Ok value
    /// @return The Ok data
    T& get_ok() {
        if (!is_ok()) {
            throw std::runtime_error("Attempting to get Ok value when not Ok");
        }
        return *reinterpret_cast<T*>(storage);
    }

    /// @brief Get the error data
    /// @return The error data
    E& get_err() {
        if (!is_err()) {
            throw std::runtime_error("Attempting to get Err value when not Err");
        }
        return *reinterpret_cast<E*>(storage);
    }


private:
    alignas(std::max(alignof(T), alignof(E)))
    std::byte storage[std::max(sizeof(T), sizeof(E))];
    
    enum class State {
        Empty,
        Ok,
        Err
    } current_state;

    /// @brief Destroy data within this storage
    void destroy() {
        switch (current_state) {
            case State::Ok:
                reinterpret_cast<T*>(storage)->~T();
                break;
            case State::Err:
                reinterpret_cast<E*>(storage)->~E();
                break;
            default:
                break;
        }

        current_state = State::Empty;
    }

    /// @brief Reset the state of this storage to empty it
    void reset() {
        current_state = State::Empty;
    }
};

template <typename T, typename E>
class Result;

template <typename T>
class Ok {
private:
    T value;

public:
    explicit Ok(T val) : value(std::move(val)) {}

    template <typename U, typename E>
    operator Result<U, E>() const {
        return Result<U, E>::template createOk(value);
    }

    // Allow access to underlying value for Result
    template <typename U, typename E>
    friend class Result;
};

template <typename E>
class Err {
private:
    E error;

public:
    explicit Err(E err) : error(std::move(err)) {}

    template <typename T, typename U>
    operator Result<T, U>() const {
        return Result<T, U>::template createErr(error);
    }

    // Allow access to underlying error for Result
    template <typename T, typename E>
    friend class Result;
};

template <typename T, typename E>
class Result {
public:
    /// @brief Default constructor
    Result() = delete;

    // Result(const Ok<T>& ok) : data(ok.value), is_ok_flag(true) {}
    // Result(Ok<T>&& ok) : data(std::move(ok.value)), is_ok_flag(true) {}
    // Result(const Err<E>& err) : data(err.error), is_ok_flag(false) {}
    // Result(Err<E>&& err) : data(std::move(err.error)), is_ok_flag(false) {}

    // /// @brief Constructor for copyable values
    // /// @tparam U 
    // /// @param value 
    // template <
    //     typename U = T,
    //     typename = std::enable_if_t<std::is_copy_constructible_v<U>>
    // >
    // Result(const T& value) 
    //     : data(value)
    //     , is_ok_flag(true)
    // {}

    // Result(Result&&) noexcept = default;
    // Result& operator=(Result&&) noexcept = default;

    // /// @brief Constructor for copyable error values
    // /// @tparam U alias for E
    // /// @tparam only enable if copyable
    // /// @param error Error to store
    // template <
    //     typename U = E,
    //     typename = std::enable_if_t<std::is_copy_constructible_v<U>>
    // >
    // Result(const E& error) 
    //     : data(error)
    //     , is_ok_flag(false)
    // {}

    // /// @brief Constructor for moveable errors
    // /// @tparam U alias for E
    // /// @tparam Only enable if move constructable
    // /// @param error The error to store
    // template <
    //     typename U = E,
    //     typename = std::enable_if_t<std::is_move_constructible_v<U>>
    // >
    // Result(E&& error)
    //     : data(std::move(error))
    //     , is_ok_flag(false)
    // {}

    // Prevent copy construction and assignment
    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;

    // Conversion constructors from Ok and Err
    Result(Ok<T>&& ok) {
        storage.store_ok(std::move(ok.value));
    }

    Result(Err<E>&& err) {
        storage.store_err(std::move(err.error));
    }

    // Check if Ok
    bool isOk() const {
        return storage.is_ok();
    }

    // Check if Err
    bool isErr() const {
        return storage.is_err();
    }

    // Unwrap Ok value
    T& unwrap() {
        return storage.get_ok();
    }

    // Unwrap Err value
    E& unwrapErr() {
        return storage.get_err();
    }

    // Pattern matching-like method
    template <typename OkFunc, typename ErrFunc>
    auto match(OkFunc okFunc, ErrFunc errFunc) {
        if (isOk()) {
            return okFunc(storage.get_ok());
        } else {
            return errFunc(storage.get_err());
        }
    }



private:
    Storage<T, E> storage;
    // std::variant<T, E> data;
    bool is_ok_flag;


    template <typename U>
    static Result createOk(U&& value) {
        Result res;
        res.storage.store_ok(std::forward<U>(value));
        return res;
    }

    template <typename U>
    static Result createErr(U&& error) {
        Result res;
        res.storage.store_err(std::forward<U>(error));
        return res;
    }
};

template <typename T>
Ok(T) -> Ok<std::decay_t<T>>;

template <typename E>
Err(E) -> Err<std::decay_t<E>>;


// // Deduction guide to help with type inference
// template <typename T>
// Result(T) -> Result<std::decay_t<T>, std::decay_t<T>>;

// template <typename T, typename E>
// class [[nodiscard]] Result;

// // For the Ok type of a result
// template <typename T>
// struct Ok {
//     explicit constexpr Ok(const T& value) : m_value(value) {}
//     explicit constexpr Ok(T&& value) : m_value(std::move(value)) {}

//     constexpr const T& value() const& { return m_value; }
//     constexpr T&& value() && { return std::move(m_value); }

//     template <typename E>
//     constexpr operator Result<T, E>() const& {
//         return Result<T, E>(Ok(m_value));
//     }
    
//     template <typename E>
//     constexpr operator Result<T, E>() && {
//         return Result<T, E>(Ok(std::move(m_value)));
//     }

// private:
//     T m_value;
// };

// // For the Err type of a result
// template <typename E>
// struct Err {
//     explicit constexpr Err(const E& value) : m_value(value) {}
//     explicit constexpr Err(E&& value) : m_value(std::move(value)) {}

//     constexpr const E& value() const& { return m_value; }
//     constexpr E&& value() && { return std::move(m_value); }

// private:
//     E m_value;
// };

// template <typename T, typename E>
// class Result {
// public:
//     using value_type = T;
//     using error_type = E;
//     static_assert(std::is_same<std::remove_reference_t<T>, T>::value,
//             "Result<T, E> cannot store reference types."
//             "Try using `std::reference_wrapper`");
//     static_assert(std::is_same<std::remove_reference_t<E>, E>::value,
//             "Result<T, E> cannot store reference types."
//             "Try using `std::reference_wrapper`");

//     static_assert(!std::is_same<T, void>::value,
//             "Cannot create a Result<T, E> object with T=void. "
//             "Introducing `void` to the type causes a lot of problems, "
//             "use the type `unit_t` instead");
//     static_assert(!std::is_same<E, void>::value,
//             "Cannot create a Result<T, E> object with E=void. You want an "
//             "optional<T>.");

//     Result() 
//         : m_ok(false) {
//     }
    
//     Result(Ok<T> value) : m_data(std::move(value.value())), m_ok(true) {}
//     Result(Err<E> value) : m_data(std::move(value.value())), m_ok(false) {}

//     // For types that are coercible like pointers to inherited classes
//     // template<typename U>
//     // Result(Ok<U> value) : m_data(static_cast<U>(value.value())), m_ok(true) {}

//     ~Result() {

//     }
//     Result(const Result& other) {
//         m_data = other.m_data;
//     }
//     Result(Result&& other) {
//         std::swap(other.m_ok, this->m_ok);
//         std::swap(other.m_data, this->m_data);
//     }

//     Result& operator=(Result other) {
//         m_data = other.m_data;
//         m_ok = other.m_ok;
//         return *this;
//     }

//     // [[ Conversion operators ]] //

//     /// @brief If Result is Ok(), return the value. 
//     /// Otherwise, panic
//     constexpr T&& unwrap() {
//         if (m_ok) {
//             return std::get<T>(std::move(m_data));
//         }

//         terminate("Panic: unwrap failed on Result");
//         std::exit(1);
//     }

//     constexpr E&& unwrap_err() {
//         if(!m_ok) {
//             this->terminate("Called `unwrap_err` on Err value");
//         }
//         return std::get<E>(std::move(m_data));
//     }

//     constexpr T&& unwrap_or(T&& val) {
//         if (m_ok) {
//             return std::get<T>(std::move(m_data));
//         }
//         return val;
//     }

//     constexpr T&& unwrap_or_default() {
//         static_assert(
//             std::is_default_constructible<T>::value,
//             "`unwrap_or_default` requires <T> to be default construbtible"
//         );

//         if(m_ok) {
//             return std::get<T>(std::move(m_data));
//         }
//         return T();
//     }

//     constexpr E&& unwrap_err_or(E && error) {
//         static_assert(
//             std::is_default_constructible<E>::value,
//             "`unwrap_or_default` requires <E> to be default construbtible"
//         );

//         if (!m_ok) {
//             return E();
//         }
//         return std::get<E>(std::move(m_data));
//     }

//     constexpr E&& expect_err(const std::string_view& msg) {
//         if(m_ok) {
//             this->terminate(msg);
//         }

//         return std::get<E>(std::move(m_data));
//     }


//     bool is_ok() const { return this->m_ok == true; }
//     bool is_err() const { return this->m_ok == false; }

// private:
//     std::variant<T, E> m_data;
//     bool m_ok;

//     /// @brief Panic and termiante program if Result is invalid
//     void terminate(const std::string& msg) {
//         std::cerr << msg << std::endl;
//         std::terminate();
//     }
// };

} // namespace result
} // gravity