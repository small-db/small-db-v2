#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace b {
    struct EmbedInternal {
        class EmbeddedFile {
            public:
            constexpr EmbeddedFile () = default;

            constexpr EmbeddedFile (const std::string_view& data, const std::string_view& filename)
            : m_data (data), m_filename (filename) {
            }

            [[nodiscard]] std::string str () const {
                return m_data.data ();
            }

            [[nodiscard]] const char* data () const {
                return m_data.data ();
            }

            [[nodiscard]] std::vector<uint8_t> vec () const {
                return { m_data.begin (), m_data.end () };
            }

            [[nodiscard]] size_t length () const {
                return m_data.size ();
            }

            [[nodiscard]] size_t size () const {
                return m_data.size ();
            }

            operator std::string () const {
                return str ();
            }

            operator std::vector<uint8_t> () const {
                return vec ();
            }

            void get (
            const std::function<void (const b::EmbedInternal::EmbeddedFile&)>& callback);

            private:
            std::string_view m_data;
            std::string_view m_filename;
        }; // class EmbeddedFile

        static EmbeddedFile simple_resources_banner_txt;

    }; // struct embedded_files

    template <size_t N> struct embed_string_literal {
        constexpr embed_string_literal (const char (&str)[N]) {
            std::copy_n (str, N, value);
        }
        constexpr bool operator!= (const embed_string_literal& other) const {
            return std::equal (value, value + N, other.value);
        }
        [[nodiscard]] std::string str () const {
            return std::string (value, N);
        }
        [[nodiscard]] constexpr bool _false () const {
            return false;
        }
        char value[N];
    };

    template <size_t N, size_t M>
    constexpr bool
    operator== (const embed_string_literal<N>& left, const char (&right)[M]) {
        return std::equal (left.value, left.value + N, right);
    }

    template <embed_string_literal identifier>
    constexpr EmbedInternal::EmbeddedFile embed () {
        if constexpr (identifier == "resources/banner.txt") {
            return EmbedInternal::simple_resources_banner_txt;
        } else {
            static_assert (identifier._false (), "[b::embed<>] No such file or directory");
        }
    }
} // namespace b

inline std::ostream&
operator<< (std::ostream& stream, const b::EmbedInternal::EmbeddedFile& file) {
    stream << file.str ();
    return stream;
}