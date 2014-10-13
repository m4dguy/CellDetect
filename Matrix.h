#ifndef MATRIX_H
#define MATRIX_H

#include "Utils.h"
#include "Vector.h"

/*
 * TODO: wtf is this?! revise!
 */

template <typename T> class Matrix
{
	public:
		Matrix(const size_t dimension=512)
        {
            this->_width = this->_height = dimension;

            this->_power = 1;
            uint trueSize = 1;
            while((trueSize<<=1) < dimension)
                ++(this->_power);

            this->_memory = trueSize * this->_height * sizeof(T);
            this->_entries = (T*) calloc(trueSize * this->_height, sizeof(T));
        };


        Matrix(const size_t width, const size_t height)
        {
            this->_width = width;
            this->_height = height;

            this->_power = 1;
			uint trueSize = 1;
			while((trueSize<<=1) < width)
                ++(this->_power);

            this->_memory = trueSize * height * sizeof(T);
            this->_entries = (T*) calloc(trueSize * height, sizeof(T));
        };

        Matrix(const Matrix& other)
        {
            this->_width = other._width;
            this->_height = other._height;
			this->_memory = other._memory;
			this->_power = other._power;

            this->_entries = (T*) malloc(this->_memory);
            memcpy(this->_entries, other._entries, other._memory);
        };

        ~Matrix()
        {
            free(_entries);
        };

        inline T& get(size_t x, size_t y)
        {
            return _entries[(y << _power) | x];
        };

        inline const T& get(size_t x, size_t y) const
        {
            return _entries[(y << _power) | x];
        };

        const T& getMirrored(int x, int y) const
        {
			int w = _width;
			int h = _height;

			x = abs(x);
            y = abs(y);
            x = (x>=w)? (w - (x - w) - 1) : x;
            y = (y>=h)? (h - (y - h) - 1) : y;
            return get(x,y);
        };

        T& getMirrored(int x, int y)
        {
			int w = _width;
			int h = _height;

			x = abs(x);
            y = abs(y);
            x = (x>=w)? (w - (x - w) - 1) : x;
            y = (y>=h)? (h - (y - h) - 1) : y;
            return get(x,y);
        };

        inline void set(size_t x, size_t y, T& val)
        {
            (*this)(x,y) = val;
        };

        inline size_t width() const
        {
            return _width;
        };

        inline size_t height() const
        {
            return _height;
        };

		void fill(T val = 0)
		{
			for(uint y=0; y<_height; ++y)
				for(uint x=0; x<_width; ++x)
					get(x,y) = val;
		};

        void resize(const size_t width, const size_t height)
        {
            this->_width = width;
            this->_height = height;

            this->_power = 1;
            uint trueSize=1;
            while((trueSize<<=1) < width)
                ++(this->_power);

			this->_memory = trueSize * height * sizeof(T);
			this->_entries = (T*) realloc(_entries, trueSize * this->_height * sizeof(T));
        };

		void swap(Matrix& other)
		{
			std::swap(_width, other._width);
			std::swap(_height, other._height);
			std::swap(_power, other._power);
			std::swap(_memory, other._memory);
			std::swap(_entries, other._entries);
		};

        inline T& operator()(size_t x, size_t y)
        {
            return get(x,y);
        }

        inline const T& operator()(size_t x, size_t y) const
        {
            return get(x,y);
        }

        const Matrix& operator=(const Matrix<T>& other)
        {
            if(this == &other)   // handle self assignment
                return *this;

            //reallocate if not enough memory
            if(other._memory > _memory)
                _entries = (T*) realloc(_entries, other._memory);

            _memory = other._memory;

            _width = other._width;
            _height = other._height;
            memcpy(_entries, other._entries, _memory);

            return *this;
        };

        Matrix<T>& operator+=(const Matrix<T>& A)
        {
            for(uint x=0; x<this->_width; ++x)
                for(uint y=0; y<this->_height; ++y)
                    get(x,y) += A(x,y);

            return *this;
        }

		Matrix<T> operator+(const Matrix<T>& A)
		{
			Matrix<T> res(_width, _height);
			for(uint x=0; x<_width; ++x)
				for(uint y=0; y<_height; ++y)
					res(x,y) = get(x,y) + A(x,y);

			return res;
		}

        Matrix<T>& operator-=(const Matrix<T>& A)
        {
            for(uint x=0; x<this->_width; ++x)
                for(uint y=0; y<this->_height; ++y)
                    get(x,y) -= A(x,y);

            return *this;
        }

		Matrix<T> operator-(const Matrix<T>& A)
		{
			Matrix<T> res(_width, _height);
			for(uint x=0; x<_width; ++x)
				for(uint y=0; y<_height; ++y)
					res(x,y) = get(x,y) - A(x,y);

			return res;
		}

        Matrix<T> operator*(const Matrix<T>& mtx)
        {
            T entry;
            const size_t width = mtx.width();
           const  size_t height = mtx.height();
            Matrix<T> res(width, height);

            for(uint j=0; j<height; ++j)
            {
                for(uint i=0; i<width; ++i)
                {
                    entry = 0;
                    for(uint k=0; k<height; ++k)
                    {
                        entry += get(k,i) * mtx(j,k);
                    }
                    res(j,i) = entry;
                }
            }
            return res;
        };

        Vector<T> operator*(const Vector<T>& vec)
        {
            T entry;
            Vector<T> res(_width);

            for(uint x=0; x<_width; ++x)
            {
                entry = 0;
                for(uint y=0; y<_height; ++y)
                {
                    entry += vec[y] * get(x,y);
                }
                res[x] = entry;
            }

            return res;
        }

        Matrix<T>& operator*=(T scalar)
        {
            size_t dim = _memory / sizeof(T);
            for(uint i=0; i<dim; ++i)
                _entries[i] *= scalar;

            return *this;
        };

		//optimize!
        Matrix<T>& operator*=(const Matrix<T>& A)
        {
            T entry;
            const size_t w = A.width();
            const size_t h = A.height();
            Vector<T> row(w);

            for(uint y=0; y<h; ++y)
            {
                for(uint x=0; x<w; ++x)
                {
                    //save row; will be overwritten otherwise
                    for(uint k=0; k<w; ++k)
                        row[k] = get(k,y);

                    entry = 0;
                    for(uint k=0; k<h; ++k)
                        entry += row[k] * A(x,k);

                    get(y,x) = entry;
                }
            }
            return *this;
        }

		Matrix<T>& operator/=(T scalar)
		{
			size_t dim = _memory / sizeof(T);
			for(uint i=0; i<dim; ++i)
				_entries[i] /= scalar;

			return *this;
		};

		bool operator==(const Matrix<T>& other) const
		{
			if((other._width != this->_width) && (other._height != this->_height))
				return 0;

			for(uint x=0; x<_width; ++x)
				for(uint y=0; y<_height; ++y)
					if(other.get(x,y) != get(x,y))
						return false;

			return true;
		};

		bool operator!=(const Matrix<T>& other) const
		{
			return !(*this == other);
		};

		//output in csv format for file dumps
		friend std::ostream& operator<<(std::ostream& stream, const Matrix<T>& mtx)
		{
			for(uint y=0; y<mtx._height; ++y)
			{
				for(uint x=0; x<mtx._width; ++x)
				{
					stream << mtx.get(x,y);

					if(mtx._width - x - 1)
						stream << ", ";
				}
				stream << std::endl;
			}
			return stream;
		};

		//use for debugging
		inline void print()
		{
			std::cout << (*this) << std::endl;
		};

	protected:
		uint _power;
		size_t _width;			//matrix dimensions
		size_t _height;

		size_t _memory;			//used for memory management and operators
		T* _entries;			//pointer to entries

	private:
};

typedef Matrix<double> MatrixD;
typedef Matrix<float> MatrixF;

#endif // MATRIX_H
