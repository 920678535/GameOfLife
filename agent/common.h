#pragma once

#include <algorithm>
#include <exception>
#include <stdint.h>
#include <vector>

namespace lulu {

struct GridPt {
    GridPt() = default;
    GridPt(int _x , int _y ):x(_x),y(_y){}
    constexpr bool valid() const noexcept
    {
        return x >= 0 && y >= 0;
    }

    std::vector<GridPt> around8()
    {
        static std::vector<GridPt> Degree_8 = {
            { -1, 0 },
            { 1, 0 },
            { 0, 1 },
            { 0, -1 },
            { -1, 1 },
            { 1, 1 },
            { -1, -1 },
            { 1, -1 }
        };
        return around(Degree_8);
    }
    std::vector<GridPt> around4()
    {
        static std::vector<GridPt> Degree_4 = {
            { -1, 0 },
            { 1, 0 },
            { 0, 1 },
            { 0, -1 }
        };
        return around(Degree_4);
    }
    std::vector<GridPt> around(const std::vector<GridPt>& _degree)
    {
        std::vector<GridPt> _res;
        _res.reserve(_degree.size());
        for (auto _offset : _degree) {
            GridPt _pt{ x + _offset.x, y + _offset.y };
            if (_pt.valid())
                _res.emplace_back(std::move(_pt));
        }
        return _res;
    }

    int x = { 0 };
    int y = { 0 };
};

template <typename T>
class Grid {
public:
    using value_type = T;
    using contain_type = std::vector<value_type>;
    Grid() noexcept = default;
    Grid(uint32_t _cellx, uint32_t _celly) noexcept
        : cellx_(_cellx)
        , celly_(_celly)
        , data_(static_cast<std::size_t>(_cellx * _celly),0)
    {
    }
    Grid(const Grid& rhs) = delete ;
    Grid& operator=(const Grid& rhs) = delete;

    Grid(Grid&& rhs)noexcept{
        rhs.swap(*this);
    }
    Grid& operator=(Grid&& rhs)noexcept{
        rhs.swap(*this);
        return *this;
    }
    constexpr bool empty()const noexcept{
        return data_.empty();
    }
    void swap(Grid& rhs)noexcept{
        using std::swap;
        swap(cellx_ , rhs.cellx_);
        swap(celly_ , rhs.celly_);
        swap(data_,rhs.data_);
    }
    value_type at(uint32_t _x, uint32_t _y) const
    {
        if (_x >= cellx_ || _y >= celly_)
            throw std::runtime_error("index overflow.");
        return data_[cellx_ * _y + _x];
    }
    value_type& at(uint32_t _x, uint32_t _y)
    {
        if (_x >= cellx_ || _y >= celly_)
            throw std::runtime_error("index overflow.");
        return data_[cellx_ * _y + _x];
    }
    void resize(uint32_t _cellx, uint32_t _celly)noexcept{
        if(_cellx == cellx_){
            data_.resize(_cellx * _celly);
        }else{
            // re-layout
            contain_type _new (_cellx * _celly,0);
            uint32_t _minCellX = std::min(_cellx,cellx_);
            uint32_t _minCellY = std::min(_celly,celly_);
            for(int x = 0 ; x < _minCellX ; ++x)
                for(int y = 0 ; y < _minCellY ; ++y)
                    _new[y * _cellx + x] = data_[y * cellx_ + x];
            data_.swap(_new);
        }
        cellx_ = _cellx;
        celly_ = _celly;
    }
    inline bool contains(const GridPt _pt)const noexcept{
        return contains(_pt.x,_pt.y);
    }
    constexpr bool contains(uint32_t _x, uint32_t _y)const noexcept{
        return _x < cellx_ && _y < celly_;
    }
private:
    uint32_t cellx_ = { 0 };
    uint32_t celly_ = { 0 };
    contain_type data_;
};
}
