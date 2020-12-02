#pragma once
#include "../Core/Common.h"
namespace Network {
    class Buffer{
    public:
        Buffer(int size) : length(0), maxLength(size), refered(false) {
            data = new char[static_cast<size_t>(size) + 1];
        }
        Buffer(const Buffer& rhs)
            : length(rhs.length), maxLength(rhs.maxLength), refered(rhs.refered) {
            if (refered)
                data = rhs.data;
            else
                data = new char[static_cast<size_t>(maxLength) + 1];
        }
        Buffer(void* offset, int size) : length(0), maxLength(size), refered(true) {
            data = (char*)offset;
        }
        ~Buffer() {
            if (!refered) delete[] data;
        }
        Buffer& operator=(const Buffer& rhs) {
            length = rhs.length;
            maxLength = rhs.maxLength;
            refered = rhs.refered;
            if (refered)
                data = rhs.data;
            else
                data = new char[maxLength + 1];
            return *this;
        }

        int GetLength() const { return length; }
        int GetMaxLength() const { return maxLength; }
        char* GetData() {
            data[length] = 0;
            return data;
        }
        const char* GetData() const {
            const_cast<char*>(data)[length] = 0;
            return data;
        }
        void Clear() { length = 0; }
        int Remove(int position, int size)  ///< returns number of bytes removed
        {
            if (position == 0 && size >= length) {
                length = 0;
                return size;
            }                                                        // clear
            if (position < 0 || position >= length) return 0;        // out of range
            if (position + size > length) size = length - position;  // cut
            memmove(&data[position], &data[position + size], length - (static_cast<size_t>(position) + size));
            length -= size;
            return size;
        }
        int Append(const void* buffer,
            int size)  ///< returns number of bytes appended
        {
            if (length == maxLength) return 0;  // full buffer
            if (length + size > maxLength) size = maxLength - length;
            memcpy(&data[length], buffer, size);
            length += size;
            return size;
        }
        int Append(int size) {
            if (length >= maxLength) return 0;  // full buffer
            if (length + size > maxLength) size = maxLength - length;
            length += size;
            return size;
        }

        void Swap(Buffer& swapbuffer) {
            // temp variables
            bool t_refered = refered;
            int t_length = length;
            int t_maxLength = maxLength;
            char* t_data = data;

            // swaping
            refered = swapbuffer.refered;
            length = swapbuffer.length;
            maxLength = swapbuffer.maxLength;
            data = swapbuffer.data;

            swapbuffer.refered = t_refered;
            swapbuffer.length = t_length;
            swapbuffer.maxLength = t_maxLength;
            swapbuffer.data = t_data;

            return;
        }

    private:
        bool refered;
        int length;
        int maxLength;
        char* data;
    };
}