#pragma once



template <class T>
class OrderQueue
{
private:
	std::vector<T> elements;

public:
	OrderQueue(void){}
	~OrderQueue(void){}

	void enqueue(T t)
	{
		elements.push_back(t);
	}

	T dequeue()
	{
		T t = elements.front();
		elements.erase(elements.begin());
		return t;
	}

	bool isEmpty()
	{
		if (elements.size() == 0)
			return true;
		else
			return false;
	}
};

