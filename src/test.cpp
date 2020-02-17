#include <memory>
#include <vector>

class test{

};

int main(void)
{
    std::vector<std::shared_ptr<test>> t;
    auto up = std::make_shared<test>();
    t.push_back(std::move(up));

    auto move = t;
}