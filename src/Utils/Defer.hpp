#pragma once
#include <functional>
#include <type_traits>

namespace details {
	struct Defer {
	private:
		std::function<void(void)> todo;

	public:
		Defer() = default;
		~Defer() {
			if (todo) todo();
		}

		template<typename Callable>
		Defer(Callable&& todo) noexcept {
			this->todo = todo;
		};
	};
};

#define defer details::Defer _CONCAT(defer_, __COUNTER__) = [&]
