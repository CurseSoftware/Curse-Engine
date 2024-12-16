#pragma once
#include <type_traits>

namespace gravity {
namespace result {

template <typename T, typename E>
class [[nodiscard]] Result;

template <typename T>
class Ok {
public:
    /// @brief Constructor for copyable types
    /// @tparam U Alias for generic type
    /// @param value Value to hold
    template <
        typename U = T,
        typename = std::enable_if_t<std::is_copy_constructible_v<U>>
    >
    explicit constexpr Ok(const T& value) : value(value) {}

    /// @brief Constructor for move-only objects
    /// @tparam U Alias for generic type
    /// @param value Value to hold 
    template <
        typename U = T,
        typename = std::enable_if_t<std::is_move_constructible_v<U>>
    >
    explicit constexpr Ok(T&& value) : value(std::move(value)) {}

    template <typename E>
    constexpr operator Result<T, E>() const& {
        return Result<T, E>(Ok(value));
    }
    template <typename E>
    constexpr operator Result<T, E>() && {
        return Result<T, E>(Ok(std::move(value)));
    }

    template <typename U, typename E>
    friend class Result;

private: 
    /// @brief Data held
    T value;
};

template <typename E>
class Err {
public:
    /// @brief Constructor or copyable types
    /// @tparam U alias for error generic
    /// @param error Error to hold
    template <
        typename U = E,
        typename = std::enable_if_t<std::is_copy_assignable_v<U>>
    > 
    explicit Err(const U& error) : value(error) {}
    
    /// @brief Constructor or move only types
    /// @tparam U alias for error generic
    /// @param error Error to hold
    template <
        typename U = E,
        typename = std::enable_if_t<std::is_move_assignable_v<U>>
    > 
    explicit Err(U&& error) : value(std::move(error)) {}

    // TODO: operator to convert to Result

    template <typename T, typename U>
    friend class Result;
private:
    E value;
};

/** STORAGE INFORMATION **/
namespace details {

    template <typename T, typename E>
    class Storage {
        using DecayT = std::decay_t<T>;
        using DecayE = std::decay_t<E>;
    public:
        using value_type = T;
        using error_type = E;
        using storage_type = std::aligned_union_t<1, T, E>;

        Storage() = delete;
        ~Storage() { destroy(); }

        /// @brief Constructor from Ok<T> class
        /// @param val Ok class to construct from
        constexpr Storage(Ok<T> val) {
            new(&storage) DecayT(std::move(val).value());
            kind = Kind::Ok;
        }

        /// @brief Constructor from Err<E> class
        /// @param err Err class to construct from
        constexpr Storage(Err<E> err) {
            new(&storage) DecayE(std::move(err).value());
            kind = Kind::Err;
        }

        /// @brief Copy constructor
        constexpr Storage(const Storage<T,E>& rhs) noexcept (
            std::is_nothrow_copy_constructible_v<T>
            && std::is_nothrow_copy_constructible_v<E>
        ) {
            kind = rhs.kind;
            switch (kind) {
                case Kind::Ok:
                    new(&storage) DecayT(rhs.get<T>());
                    break;
                case Kind::Err:
                    new(&storage) DecayE(rhs.get<E>());
                    break;
            }
        }

        /// @brief Move constructor
        constexpr Storage(Storage&& rhs) noexcept (
            std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>
        ) {
            kind = rhs.kind;
            switch (kind) {
                case Kind::Ok:
                    new(&storage) DecayT(std::move(rhs).template get<T>());
                    break;
                case Kind::Err:
                    new(&storage) DecayE(std::move(rhs).template get<E>());
                    break;
            }
        }

        /// @brief Copy assignment operator
        constexpr Storage& operator=(const Storage<T,E> &rhs) noexcept (
            std::is_nothrow_copy_constructible_v<T>
            && std::is_nothrow_copy_constructible_v<E>
        ) {
            destroy();
            kind = rhs.kind;
            switch (kind) {
                case Kind::Ok: {
                    T& val = get<T>();
                    val = rhs.get<T>();
                } break;
                
                case Kind::Err: {
                    E& val = get<E>();
                    val = rhs.get<E>();
                } break;
            }
        }

        /// @brief Move assignment operator
        constexpr Storage&& operator=(Storage<T,E> &&rhs) noexcept (
            std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>
        ) {
            destroy();
            kind = rhs.kind;
            switch (kind) {
                case Kind::Ok: {
                    T& val = get<T>();
                    val = std::move(rhs).template get<T>();
                } break;
                
                case Kind::Err: {
                    E& val = get<E>();
                    val = std::move(rhs).template get<E>();
                } break;
            }
        }

        // GETTERS

        /// @brief Getter for a const reference to the storage<U>
        /// @tparam U type param we want from the storage
        /// @return constant reference to the data in storage
        template <typename U> 
        constexpr const U& get() const& noexcept {
            static_assert(
                std::is_same_v<T, U> || std::is_same_v<E, U>
                , "get<U> call where U != T and U != E"
            );
            return *reinterpret_cast<const U*>(&storage);
        }

        /// @brief Getter for reference to the data in storage
        /// @tparam U type param we want from storage
        /// @return reference to the data
        template <typename U> 
        constexpr U& get() & noexcept {
            static_assert(
                std::is_same_v<T, U> || std::is_same_v<E, U>
                , "get<U> call where U != T and U != E"
            );
            return *reinterpret_cast<U*>(&storage);
        }

        /// @brief Getter for moving the data
        /// @tparam U type param we want from storage
        /// @return the moved data
        template <typename U> 
        constexpr U&& get() && noexcept {
            static_assert(
                std::is_same_v<T, U> || std::is_same_v<E, U>
                , "get<U> call where U != T and U != E"
            );
            return std::move(*reinterpret_cast<const U*>(&storage));
        }

    private:
        /// @brief Stores the data
        storage_type storage;

        /// @brief The state the result is in
        enum class Kind {
            Ok,
            Err
        } kind;

        /// @brief Destroy the values within the storage
        void destroy() {
            switch (kind) {
                case Kind::Ok:
                    get<T>().~T();
                    break;
                case Kind::Err:
                    get<E>().~E();
                    break;
            }
        }
    };

} // namespace details

template <typename T, typename E>
class [[nodiscard]] Result {
public:
    using value_type = T;
    using error_type = E;

    static_assert(
        std::is_same_v<std::remove_reference_t<T>, T>,
        "Result<T,E> cannot store reference types"
    );
    static_assert(
        std::is_same_v<std::remove_reference_t<E>, T>,
        "Result<T,E> cannot store reference types"
    );
    static_assert(
        std::is_same_v<void, T>,
        "Cannot create Result<void, E>"
    );
    static_assert(
        std::is_same_v<void, E>,
        "Cannot create Result<T, void>. Use std::optional for this bevavior"
    );

    constexpr Result() {
        static_assert(
            std::is_default_constructible_v<T>,
            "Result<T,E> can only be default constructible if T is default constructible"
        );

        storage = Ok(T());
    }

private:
    details::Storage<T,E> storage;
};


} // result namespace
} // gravity namespace