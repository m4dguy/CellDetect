#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>

#include "Utils.h"

/*
 * TODO: wtf is this?! revise!
 */

template <typename T> class Vector
{
    public:
        Vector(size_t dimension)
        {
            _dimension = dimension;
            _entries = (T*) calloc(_dimension, sizeof(T));
        };

        Vector(const Vector& other)
        {
            _dimension = other._dimension;
			_entries = (T*) calloc(other._dimension, sizeof(T));
            memcpy(_entries, other._entries, _dimension * sizeof(T));
        };

        ~Vector()
		{
			free(_entries);
		};

        inline size_t size() const
		{
			return _dimension;
		};

		void fill(T val = 0)
		{
			for(uint i=0; i<_dimension; ++i)
				_entries[i] = val;
		};

		void resize(size_t dimension)
        {
			this->_dimension = dimension;
			if(dimension > _dimension)
				_entries  = (T*) realloc(_entries, dimension * sizeof(T));
        };

        Vector& operator=(const Vector& other)
        {
            if(this == &other)   // handle self assignment
                return *this;

            //reallocate if not enough space
            if(other._dimension > this->_dimension)
                _entries = (T*) realloc(_entries, other._dimension * sizeof(T));

            _dimension = other._dimension;
            memcpy(_entries, other._entries, _dimension * sizeof(T));

            return *this;
        };

        Vector<T>& operator*=(T& scalar)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] *= scalar;

            return *this;
        };

        Vector<T>& operator*=(const T& scalar)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] *= scalar;

            return *this;
        };

        Vector<T>& operator/=(T& scalar)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] /= scalar;

            return *this;
        };

        Vector<T>& operator/=(const T& scalar)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] /= scalar;

            return *this;
        };

        Vector<T>& operator+=(const Vector<T>& other)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] += other._entries[i];

            return *this;
        }

        Vector<T>& operator-=(const Vector<T>& other)
        {
            for(uint i=0; i<_dimension; ++i)
                _entries[i] -= other._entries[i];

            return *this;
        }

        bool operator==(const Vector<T>& other) const
        {
            if(other._dimension != _dimension)
                return 0;

            bool eq = 1;

            for(uint i=0; (eq && i<_dimension); ++i)
                eq &= (other[i] == (*this)[i]);

            return eq;
        };

        bool operator!=(const Vector<T>& other) const
        {
            return !(*this == other);
        };

        inline const T& operator[](size_t i) const {return _entries[i];};
        inline T& operator[](size_t i){return _entries[i];};

        friend std::ostream& operator<<(std::ostream& stream, const Vector<T>& vec)
        {
            for(uint i=0; i<vec._dimension; ++i)
                stream << vec[i] << ", ";

            return stream;
        };

        inline void print(){std::cout << (*this) << std::endl;};

    protected:
        size_t _dimension;
        T *_entries;

    private:
};

typedef Vector<double> VectorD;
typedef Vector<float> VectorF;

#endif // VECTOR_H
