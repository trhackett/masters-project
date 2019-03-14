
#include <iostream>
#include <queue>
#include <iomanip>
using namespace std;

struct node {
	int row, col;
	double g, h, f;
	node(int r, int c, double g_, double h_) {
		row = r; col = c; g = g_; h = h_; f = g+h;
	}
};


  // https://en.cppreference.com/w/cpp/container/priority_queue
  // https://en.cppreference.com/w/cpp/language/decltype
int main() {

	node n1(0, 0, 1.5, 2.4);
	node n2(3, 3, 5.5, 6.8);

	bool (*cmp) (const node&, const node&) =
		[] (const node& left, const node& right)
		{ return left.f > right.f; };

	priority_queue<node, vector<node>, decltype(cmp)> pq(cmp);
	pq.push(n1);
	pq.push(n2);

	while (!pq.empty()) {
		double f = pq.top().f;
		cout << f << " ";
		pq.pop();
	}
	cout << endl;
}


/*
#include <iostream>
#include <queue>
#include <vector>
#include <utility>
 
using my_pair_t = std::pair<size_t,bool>;
 
using my_container_t = std::vector<my_pair_t>;
 
int main()
{
    auto my_comp =
        [](const my_pair_t& e1, const my_pair_t& e2) 
        { return e1.first > e2.first; };
    std::priority_queue<my_pair_t,
                        my_container_t,
                        decltype(my_comp)> queue(my_comp);
    queue.push(std::make_pair(5, true));
    queue.push(std::make_pair(3, false));
    queue.push(std::make_pair(7, true));
    std::cout << std::boolalpha;
    while(!queue.empty()) 
    {
        const auto& p = queue.top();
        std::cout << p.first << " " << p.second << "\n";
        queue.pop();
    }
}
*/