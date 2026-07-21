#pragma once

#include "common/domain/vo/PagingVo.hpp"
#include <string>

class PageUtils{
public:
	PageUtils(int32_t page = 1, int32_t pageSize = 10, int32_t totalCount = -1)
		: _page(page)
		, _pageSize(std::min(pageSize, 100))
		, _totalCount(totalCount)
	{
	}

	template<typename T>
	std::shared_ptr<PagingVo<T>> getPageData(const std::vector<std::shared_ptr<T>>& list)
	{
		auto paging = std::make_shared<PagingVo<T>>();
		if (list.empty()) {
			return paging;
		}
		if (offset() > list.size()) {
			return paging;
		}
		paging->page = _page;
		paging->pageSize = _pageSize;
		paging->totalCount = _totalCount == -1 ? list.size() : _totalCount;

		auto max = std::min((size_t)limit(), list.size() - offset());
		for (size_t i = offset(); i < offset() + max; i++) {
			paging->items.push_back(std::move(list.at(i)));
		}
		return paging;
	}

	inline int32_t offset()const {
		return (_page - 1) * _pageSize;
	}
	inline int32_t limit()const {
		return _pageSize;
	}
private:
	int32_t _page;
	int32_t _pageSize;
	int32_t _totalCount;
};
