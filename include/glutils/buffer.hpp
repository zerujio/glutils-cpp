#ifndef GLUTILS_BUFFER_HPP
#define GLUTILS_BUFFER_HPP

#include "handle.hpp"
#include "object.hpp"

#include <utility>
#include <vector>

namespace GL {

/// Wraps "NamedBuffer" OpenGL functions.
class BufferHandle : public Handle
{
    using Handle::Handle;
public:
    static auto create() -> BufferHandle;

    static void destroy(BufferHandle buffer);

    // represents a memory range within a buffer
    struct Range
    {
        GLintptr offset{0};
        GLsizeiptr size{0};
    };

    enum class Parameter : GLenum
    {
        Access = 0x88BB,
        AccessFlags = 0x911F,
        Immutable = 0x821F,
        Mapped = 0x88BC,
        MapLength = 0x9120,
        MapOffset = 0x9121,
        Size = 0x8764,
        StorageFlags = 0x8220,
        Usage = 0x8765
    };

    /// Return parameters of a buffer object
    /**
     * Wraps glNamedBufferParameteriv().
     *
     * @param pname the enum value of the parameter to query.
     * @return the current value of the parameter.
     */
    [[nodiscard]]
    auto getParameter(Parameter pname) const -> GLint;

    /// Same as getParameter(), but returns a 64 bit integer.
    [[nodiscard]]
    auto getParameter64(Parameter pname) const -> GLint64;

    /// Access modes for glMapBuffer.
    enum class AccessMode : GLenum
    {
        read_only = 0x88B8,
        write_only = 0x88B9,
        read_write = 0x88BA
    };

    /// Query the GL_BUFFER_ACCESS parameter.
    /**
     * If the buffer was mapped with glMapBufferRange(), the access policy is determined by translating the bits in
     * that access parameter to one of the supported enums for glMapBuffer() as described in the OpenGL Specification.
     *
     * @return the access policy set while mapping the buffer object (the value of the @p access parameter bitfield
     * passed to glMapBuffer).
     */
    [[nodiscard]] AccessMode getAccessMode() const;

    /// Access flags for glMapBufferRange.
    enum class AccessFlags : GLenum
    {
        none = 0x0000,
        read = 0x0001,
        write = 0x0002,
        persistent = 0x0040,
        coherent = 0x0080,
        invalidate_range = 0x0004,
        invalidate_buffer = 0x0008,
        flush_explicit = 0x0010,
        unsynchronized = 0x0020
    };

    /// Query the GL_BUFFER_ACCESS_FLAGS parameter.
    /**
     * If the buffer was mapped with glMapBuffer, the access policy is determined by translating the enums in that
     * access parameter to the corresponding bits for glMapBufferRange as described in the OpenGL Specification. The
     * initial value is zero.
     *
     * @return the access policy set while mapping the buffer object (the value of the @p access parameter bitfield
     * passed to glMapBufferRange).
     */
    [[nodiscard]]
    auto getAccessFlags() const -> AccessFlags;

    /// Query the GL_BUFFER_IMMUTABLE_STORAGE parameter.
    /**
     * The initial value is GL_FALSE.
     *
     * @return boolean flag indicating whether the buffer object is immutable.
     */
    [[nodiscard]]
    auto getImmutable() const -> bool;

    /// Query the GL_BUFFER_MAPPED parameter.
    /**
     * The initial value is GL_FALSE.
     *
     * @return a flag indicating whether the buffer object is currently mapped.
     */
    [[nodiscard]]
    auto getMapped() const -> bool;

    /// Query the GL_BUFFER_MAP_LENGTH parameter.
    /**
     * The i64v versions of these queries should be used for this parameter. The initial value is zero.
     *
     * @return the length of the mapping into the buffer object established with glMapBuffer*.
     */
    [[nodiscard]]
    auto getMapLength() const -> GLsizeiptr;

    /// Query the GL_BUFFER_MAP_OFFSET parameter.
    /**
     * The initial value is 0.
     * @return the size of the buffer object, measured in bytes.
     */
    [[nodiscard]]
    auto getMapOffset() const -> GLintptr;

    /// Query the GL_BUFFER_SIZE parameter.
    /**
     * The initial value is 0.
     *
     * @return the size of the buffer object, measured in bytes. The
     */
    [[nodiscard]]
    auto getSize() const -> GLsizeiptr;

    enum class Usage : GLenum
    {
        static_draw = 0x88E4,
        static_read = 0x88E5,
        static_copy = 0x88E6,
        dynamic_draw = 0x88E8,
        dynamic_read = 0x88E9,
        dynamic_copy = 0x88EA,
        stream_draw = 0x88E0,
        stream_read = 0x88E1,
        stream_copy = 0x88E2
    };

    /// Query the GL_BUFFER_USAGE parameter.
    /**
     *  The initial value is GL_STATIC_DRAW.
     *
     * @return the buffer object's usage pattern.
     */
    [[nodiscard]]
    auto getUsage() const -> Usage;

    enum class StorageFlags : GLenum
    {
        none = 0x0000,
        dynamic_storage = 0x0100,
        map_read = 0x0001,
        map_write = 0x0002,
        map_persistent = 0x0040,
        map_coherent = 0x0080,
        client_storage = 0x0200,
    };

    /// Query the GL_BUFFER_STORAGE_FLAGS parameter.
    /**
     *  If the buffer object is immutable, the value returned will be that specified when the data store was
     *  established with glBufferStorage. If the data store was established with glBufferData, the value will
     *  be GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT.
     *
     *  The initial value is zero.
     *
     * @return a bitfield indicating the storage flags for the buffer object.
     */
    [[nodiscard]]
    auto getStorageFlags() const -> StorageFlags;

    /// (Re)allocate and optionally initialize mutable storage.
    /**
     * Wraps glBufferData.
     * @param size Size to allocate, in bytes.
     * @param usage Usage enum.
     * @param init_data Data to initialize the storage with, if not null.
     */
    void allocate(GLsizeiptr size, Usage usage, const void *init_data = nullptr) const;

    /// Allocate and optionally initialize immutable (non-resizable) storage.
    /**
     * Wraps glBufferStorage.
     * @param size Size to allocate, in bytes.
     * @param flags storage flags.
     * @param init_data data to initialize the store with, if not null.
     */
    void allocateImmutable(GLsizeiptr size, StorageFlags flags, const void *init_data = nullptr) const;

    /// Copy data from host memory to GPU buffer.
    /**
     * Wraps glBufferSubData
     * @param offset byte offset into the buffer storage.
     * @param size in bytes.
     * @param data pointer to the data to write.
     */
    void write(GLintptr offset, GLsizeiptr size, const void *data) const;

    void write(Range range, const void *data) const
    { write(range.offset, range.size, data); }

    /// Copy data from GPU buffer to host memory.
    /**
     * Wraps glGetBufferSubData.
     * @param offset byte offset into the buffer storage.
     * @param size in bytes.
     * @param data pointer to host memory; data read from the buffer will be written here.
     */
    void read(GLintptr offset, GLsizeiptr size, void *data) const;

    void read(Range range, void *data) const
    { read(range.offset, range.size, data); }

    /// Map the whole buffer to the host address space.
    /**
     * Wraps glMapBuffer.
     * @param access Access policy.
     * @return a valid pointer to the mapped buffer memory, or a null pointer if an error occurred.
     */
    [[nodiscard]]
    auto map(AccessMode access) const -> void *;

    /// Map a range within the buffer to the host address space.
    /**
     * Wraps glMapBufferRange.
     * @param offset byte offset into the buffer.
     * @param length in bytes of the region to map.
     * @param access access policy flags.
     * @return a valid pointer to the mapped buffer memory, or a null pointer if an error occurred.
     */
    [[nodiscard]]
    auto mapRange(GLintptr offset, GLsizeiptr length, AccessFlags access) const -> void *;

    [[nodiscard]]
    auto mapRange(Range range, AccessFlags access) const -> void *
    {
        return mapRange(range.offset, range.size, access);
    }

    /// Unmap this buffer.
    /**
     * Wraps glUnmapBuffer.
     *
     * This invalidates the pointer returned by map() and map().
     */
    void unmap() const;

    enum class IndexedTarget : GLenum
    {
        atomic_counter = 0x92C0,
        transform_feedback = 0x8C8E,
        uniform = 0x8A11,
        shader_storage = 0x90D2
    };

    /// glBindBufferBase - bind a buffer object to an indexed buffer target. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml
    void bindBase(IndexedTarget target, GLuint index) const;

    /// glBindBufferRange - bind a range within a buffer object to an indexed buffer target. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferRange.xhtml
    void bindRange(IndexedTarget target, GLuint index, GLintptr offset, GLsizeiptr size) const;

    void bindRange(IndexedTarget target, GLuint index, Range range) const
    {
        bindRange(target, index, range.offset, range.size);
    }

    /// glBindBuffersRange — bind ranges of one or more buffer objects to a sequence of indexed buffer targets.
    /**
     * https://registry.khronos.org/OpenGL-Refpages/gl4/
     * @tparam InputIter An input iterator to a pair-like of GL::BufferHandle and GL::Buffer::Range
     * @param target Indexed target to bind to.
     * @param first_binding Starting binding index.
     * @param begin Begin iterator.
     * @param end End iterator.
     */
    template<typename InputIter>
    static void bindRanges(IndexedTarget target, GLuint first_binding, InputIter begin, InputIter end)
    {
        std::vector<GLuint> buffers;
        std::vector<GLintptr> offsets;
        std::vector<GLintptr> sizes;
        GLsizei count = 0;

        auto iter = begin;
        while (iter != end)
        {
            const auto [buffer, range] = *iter++;
            buffers.emplace_back(buffer.getName());
            offsets.emplace_back(range.offset);
            sizes.emplace_back(range.size);
            count++;
        }

        s_bindRange(target, first_binding, count, buffers.data(), offsets.data(), sizes.data());
    }

    /// glBindBuffersBase — bind one or more buffer objects to a sequence of indexed buffer targets. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffersBase.xhtml
    template<typename InputIter>
    static void bindBases(IndexedTarget target, GLuint first_binding, InputIter begin_buffer, InputIter end_buffer)
    {
        std::vector<GLuint> buffers;
        GLsizei count = 0;

        auto iter = begin_buffer;
        while (iter != end_buffer)
        {
            buffers.emplace_back(iter++->getName());
            count++;
        }

        s_bindBases(target, first_binding, count, buffers.data());
    }

    static void copy(BufferHandle read_buffer, BufferHandle write_buffer, GLintptr read_offset, GLintptr write_offset,
                     GLsizeiptr size);

private:
    static void s_bindRange(IndexedTarget target, GLuint first_binding, GLsizei count,
                            const GLuint *buffers, const GLintptr *offsets, const GLintptr *sizes);

    static void s_bindBases(IndexedTarget target, GLuint first_binding, GLsizei count, const GLuint *buffers);
};

using Buffer = Object<BufferHandle>;

auto operator|(BufferHandle::AccessFlags l, BufferHandle::AccessFlags r) -> BufferHandle::AccessFlags;

auto operator&(BufferHandle::AccessFlags l, BufferHandle::AccessFlags r) -> BufferHandle::AccessFlags;

auto operator|(BufferHandle::StorageFlags l, BufferHandle::StorageFlags r) -> BufferHandle::StorageFlags;

auto operator&(BufferHandle::StorageFlags l, BufferHandle::StorageFlags r) -> BufferHandle::StorageFlags;

} // GL

#endif //GLUTILS_BUFFER_HPP
