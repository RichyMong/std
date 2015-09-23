#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

class MinStack {
public:
    void push(int x) {
        data_.push(x);
        if (min_.empty() || x <= getMin()) {
            min_.push(x);
        }
    }

    void pop() {
        if (data_.top() == getMin()) min_.pop();
        data_.pop();
    }

    int top() {
        return data_.top();
    }

    int getMin() {
        return min_.top();
    }

private:
    stack<int> data_;
    stack<int> min_;
};

int main()
{
    auto ms = MinStack();
    ms.push(-2);
    ms.push(0);
    ms.push(-1);
    cout << ms.getMin() << endl;
    cout << ms.top() << endl;
    ms.pop();
    cout << ms.getMin() << endl;
}
