#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

template<class T> 
class SmartPtr
{
public:

	SmartPtr() : value(0), count(0)
    {
    }

	SmartPtr(T* ptr)
	{
        value = ptr;
        if (value != 0)
        {
            count = new unsigned int;
            (*count) = 1;
        }
        else
        {
            count = 0;
        }   
	}

	SmartPtr(const SmartPtr& other)
        : value(0), count(0)
	{
	    value = other.value;
	    count = other.count;

        if (count)
        {
            (*count)++;
        }
	}

	template<class OTHER>
	SmartPtr(const SmartPtr<OTHER>& other)
        : value(0), count(0)
	{
        value = other.value;
        count = other.count;

        if (count)
        {
            (*count)++;
        }
	}

    SmartPtr& operator=(const SmartPtr& other) 
    {
        if (value == other.value)
            return *this;

        SmartPtr<T> tmp(other);
        swap(tmp);

        return *this;
    }

	template<class OTHER>
	SmartPtr& operator=(const SmartPtr<OTHER>& other) 
    {
		if (value == other.value)
			return *this;

		SmartPtr<T> tmp(other);
		swap(tmp);

		return *this;
	}

	virtual ~SmartPtr() 
    {
        release();
	}

	inline T& operator*() const
    { 
        return *value; 
    }

	inline T* operator->() const 
    { 
        return value; 
    }

	inline unsigned int references() const 
    { 
        return *count; 
    }

	inline T* get() const 
    { 
        return value; 
    }

	inline bool isNull(void) const 
    { 
        return value == 0; 
    }

    inline void setNull() 
    { 
	    if (value)
	    {
		    release();
		    value = 0;
		    count = 0;
	    }
    }

protected:

    template <class VALUE>
    void swap(VALUE& first, VALUE& second)
    {
        VALUE tmp = first;
        first = second;
        second = tmp;
    } 

	void swap(SmartPtr<T>& other) 
	{
        swap(value, other.value);
        swap(count, other.count);
	}

    inline void release(void)
    {
	    if (count)
	    {
		    if (--(*count) == 0) 
		    {
			    delete value;
                delete count;
            }
            value = 0;
            count = 0;
	    }
    }

private:
	T* value;
	unsigned int* count;
};

template<class T, class U> inline bool operator==(SmartPtr<T> const& first, SmartPtr<U> const& second)
{
	return first.get() == second.get();
}

template<class T, class U> inline bool operator!=(SmartPtr<T> const& first, SmartPtr<U> const& second)
{
	return first.get() != second.get();
}

#endif


