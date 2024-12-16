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

    /// @brief The state of what is being stored
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

} // namespace result
} // gravity